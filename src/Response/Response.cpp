//#include "../inc/Response.hpp"
#include "webserv.hpp"
#include <sstream>

Response::~Response(){}

Response::Response()
	: _redirectDest(""), _setCookieValue("") {
	// Default constructor implementation
}

void Response::setReasonPhrase(const std::string &reasonPhrase) {
	_reasonPhrase = reasonPhrase;
}

std::string Response::getReasonPhrase() const {
	return _reasonPhrase;
}

std::string Response::getRedirectDest() {
	return _redirectDest;
}

void Response::matchServerBlock(const std::vector<ServerConf>& serverConfs) {
	// match server block based on port
	for (size_t i = 0; i < serverConfs.size(); i++) {
		// server block we are currently inspecting

		// get ip ports map
		const std::map<std::string, std::set<int> > ipsPorts =  serverConfs[i].getIpPort();
		// loop through map
		for (std::map<std::string, std::set<int> >::const_iterator it = ipsPorts.begin(); it != ipsPorts.end(); ++it) {
			// check if we can find port in set
			if (it->second.find(_request.getPort()) != it->second.end()) {
				_serverConf = serverConfs[i];
			}
		}
	}
	throw 500;
}

void Response::matchLocationConf(void) {
	const std::vector<LocationConf>& locations = _serverConf.getLocationConfs();

	std::string uri = _request.getUri();
	// Find best matching location (longest prefix match)
	std::string bestMatch = "";

	for ( size_t i = 0; i < locations.size(); i++ ) {
		std::string locPath = locations[i].getPath();

		if (uri.find(locPath) == 0) {  // URI starts with location path
			if (locPath.length() > bestMatch.length()) {
				bestMatch = locPath;
				_locationConf = _serverConf.getLocationConfs()[i];
			}
		}
	}
	throw 500;
}

void Response::matchServerName( void ) {

	std::string hostname = _request.getHostname();

	std::vector<std::string> serverNames = _serverConf.getServerConfNames();
	// if we can find req hostname match with string in serverNames
 	std::vector<std::string>::iterator it =  std::find(serverNames.begin(), serverNames.end(), hostname);

	_serverName = ( it != serverNames.end() ) ?  hostname : "Webserv";
}

Response::Response(HttpRequest& reqObj, const std::vector<ServerConf>& serverConfs)
	: _redirectDest(""), _setCookieValue(""), _request(reqObj) {

	// check if request flagged as non 200 val
	if (! (reqObj.getResponseCode() >= 200 && reqObj.getResponseCode() <= 302) )
		throw reqObj.getResponseCode();
	// set status code to state from req obj
	setStatusCode(reqObj.getResponseCode());

	// match and set server block
	matchServerBlock(serverConfs);

	// match server name if exists
	matchServerName();

	// match and set location block
	matchLocationConf();
}

void Response::generateHttpResponse() {
	std::stringstream header;
	header << generateStatusLine() << "\r\n";
	header << "Date: " << getCurrentDateTime() << "\r\n";
	header << "Server: " << _serverName << "\r\n";
	header << "Content-Type: " << getContentType() << "\r\n";
	header << "Content-Length: " << intToString(getBody().length()) << "\r\n";
	header << "Connection: " << _request.getConnectionType() << "\r\n";

	if (!(_setCookieValue.empty())) {
		header << "Set-Cookie: " << _setCookieValue << "\r\n";
	}
	// to set location (esp for redirects)
	if ( (!(_redirectDest.empty())) && (_statusCode == 301 || _statusCode == 302)) {
		header << "Location: " << getRedirectDest() << "\r\n";
	}

	header << "\r\n"; // End of headers
	std::string responseString = header.str() + getBody();
	setHttpResponse(responseString);
}

// set the Response to a certain code (template)
void Response::setBodyErrorPage(int httpCode) {
	setBody("<html><body><h1>"
	+ Response::intToString(httpCode)
	+ " "
	+ generateReasonPhrase(httpCode)
	+ "Error "
	+ intToString(httpCode)
	+ "</h1></body></html>\n");
}

void Response::generateErrorResponse(HttpRequest &reqObj) {

	// Retrieve the error pages map
	const std::map<int, std::string>& errorPages = _serverConf->getErrorPages();

	// check request obj for the code
	int responseCode = reqObj.getResponseCode();
	std::map<int, std::string>::const_iterator customPageIt = errorPages.find(responseCode);

	// generate correct headers for our error page
	generateHeader(reqObj);

	// check if we have a custom error page for our httpCode
	if (customPageIt != errorPages.end()) {

		// Custom error page found, load and set it as the response body
		std::string errorPagePath = "." + customPageIt->second; // Use relative path
		std::ifstream file(errorPagePath.c_str());

		// if we can open the error page file
		if (file) {
			std::ostringstream contents;
			contents << file.rdbuf();
			file.close();
			setBody(contents.str());
		//updates the path to the one for the error pages
		PathInfo errorPath(errorPagePath);
		errorPath.parsePath();
		reqObj.setPathInfo(errorPath);
		// return internal server error 500 we are not able to open the file
		} else {
			setBodyErrorPage(500);
		}

		return;
	}

	// in case that no custom error page exists
	// also set error code to 500 for internal server error
	// client errors 400 - 420
	// server errors 500 - 511
	setBodyErrorPage(responseCode);
}

bool Response::isCgiRequest(const std::string& uri) {
    // Check if URI matches CGI patterns
    if (_locationConf &&
        !_locationConf->getCgiPath().empty() &&
        !_locationConf->getCgiExt().empty())  {

        // Strip query parameters by finding the first '?'
        std::string path = uri;
        size_t queryPos = path.find('?');
        if (queryPos != std::string::npos) {
            path = path.substr(0, queryPos);
        }

        // Find file extension in the path (without query parameters)
        size_t dot = path.rfind('.');
        if (dot != std::string::npos) {
            std::string ext = path.substr(dot);

            // Get allowed CGI extensions
            const std::vector<std::string>& cgiExts = _locationConf->getCgiExt();

            // Check if extension is allowed
            return (std::find(cgiExts.begin(), cgiExts.end(), ext) != cgiExts.end());
        }
    }
    return false;
}

void		Response::processResponse(HttpRequest &ReqObj){

	try {
		if (!_serverConf)
			throw 500;
		std::string fullPath = _serverConf->getRootDir() + ReqObj.getUri();
		PathInfo pathInfo(fullPath);
		pathInfo.validatePath();
		ReqObj.setPathInfo(pathInfo);
		// Validate and parse the path

		// CGI REQUEST CHECK HANDLER
		// note this is the only part of the check that returns
		if(isCgiRequest(ReqObj.getUri())){ //should be ok but some errro with cgi handler cant find files
			std::cout << "CGI REQUEST" << '\n';
			int result = handleCgi(ReqObj);
			//std::cout << "CGI result" << result << '\n';
			if(result == 0) {
				setBody(ReqObj.getCgiResponseString());
			}
			return;
		}

		// redirect request would override all request heirarchy
		// for example we could perform a GET, POST or DELETE in an old folder and that request would need to be redirected and passed on
		// get map of location redirects
		std::map<std::string, std::string > locationRedirect;
		if (_locationConf)
			locationRedirect = _locationConf->getAllowedRedirects();
		// get map of server redirects
		std::map<std::string, std::string > serverRedirects = _serverConf->getAllowedRedirects();

		// handlers
		if (!locationRedirect.empty() || !serverRedirects.empty()) {
			std::cout << "REDIRECT" << '\n';
			// ternary operator evaluate which one to pass to handle redirect
			std::map<std::string, std::string > redirect = locationRedirect.empty() ? serverRedirects : locationRedirect;
			HandleRedirectRequest(ReqObj, redirect);
		}

		// GET REQUEST HANDLER
		else if (ReqObj.getMethod() == "GET") {
			std::cout << "GET REQUEST" << '\n';
			HandleGetRequest(ReqObj,pathInfo);
		}
		// POST REQUEST HANDLER
		else if (ReqObj.getMethod() == "POST") {
			std::cout << "POST REQUEST" << '\n';
			HandlePostRequest(ReqObj, pathInfo);
		}
		// HANDLE DELETE request
		else if (ReqObj.getMethod() == "DELETE") {
			std::cout << "DELETE REQUEST" << '\n';
			HandleDeleteRequest(ReqObj, pathInfo);
		}

	}
	// catches the error and sets the code in the response code Obj
	catch(int error)
	{
		ReqObj.setResponseCode(error);
		generateErrorResponse(ReqObj);
	}

}

void Response::setSetCookieValue(std::string value) {
	_setCookieValue = value;
}