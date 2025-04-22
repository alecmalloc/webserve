//#include "../inc/Response.hpp"
#include "webserv.hpp"
#include <sstream>

Response::~Response(){}

Response::Response()
	: _serverConf(NULL), _locationConf(NULL), _redirectDest("") ,_setCookieValue("") {
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

Response::Response(HttpRequest& reqObj, ServerConf* serverConf)
	: _serverConf(serverConf), _locationConf(NULL), _redirectDest(""), _setCookieValue("") {
	// Get locations from server config
	std::string uri = reqObj.getUri();

	const std::vector<LocationConf>& locations = serverConf->getLocationConfs();

	// Find best matching location (longest prefix match)
	std::string bestMatch = "";

	for (std::vector<LocationConf>::const_iterator it = locations.begin();
		 it != locations.end(); ++it) {
		std::string locPath = it->getPath();

		if (uri.find(locPath) == 0) {  // URI starts with location path
			if (locPath.length() > bestMatch.length()) {
				bestMatch = locPath;
				_locationConf = const_cast<LocationConf*>(&(*it));
			}
		}
	}

	processResponse(reqObj);
	generateHttpresponse(reqObj);
}



void Response::generateHttpresponse(HttpRequest &reqObj) {
	generateHeader(reqObj);
	std::map<std::string, std::string> headerMap = getHeaderMap();
	std::stringstream header ;
	header << headerMap["Status-Line"] << "\r\n";
	header << "Date: " << headerMap["Date"] << "\r\n";
	header << "Server: " << headerMap["Server"] << "\r\n";
	header << "Content-Type: " << headerMap["Content-Type"] << "\r\n";
	header << "Content-Length: " << headerMap["Content-Length"] << "\r\n";
	header << "Connection: " << headerMap["Connection"] << "\r\n";
	//header << "Cache-Control: " << headerMap["Cache-Control"] << "\r\n";

	// needed for cookies to work
	// only if setCookieValue has been changed
	if (_setCookieValue != "") {
		header << "Set-Cookie: " << headerMap["Set-Cookie"] << "\r\n";
	}
	// to set location (esp for redirects)
	if (_redirectDest != "") {
		header << "Location: " << headerMap["Location"] << "\r\n";
	}
	// std::cout << "SetCookie found: " << headerMap["Set-Cookie"] << '\n';

	//header << "Last-Modified: " << headerMap["Last-Modified"] << "\r\n";
	//header << "ETag: " << headerMap["ETag"] << "\r\n";
	//header << "Location: " << headerMap["Location"] << "\r\n";
	header << "\r\n"; // End of headers
	std::string responseString = header.str() + getBody();
	setHttpResponse(responseString);
}

std::string Response::getServerName() {
	std::vector<std::string> server = _serverConf->getServerConfNames();

	// Check if the vector has any elements
	if (!server.empty()) {
		// Return the first server name from the vector
		return server[0];
	}
	//else default value and return
	return("Webserv/1.0");
}

// set the Response to a certain code (template)
void Response::setBodyErrorPage(int httpCode) {
	setBody("<html><body><h1>"
	+ Response::intToString(httpCode)
	+ " "
	+ genarateReasonPhrase(httpCode)
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
		PathInfo pathInfo = ReqObj.getPathInfo();
		pathInfo.validatePath();
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