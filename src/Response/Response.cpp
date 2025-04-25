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

HttpRequest&	Response::getHttpRequest( void ){
	return( _request );
}

ServerConf	Response::getServerConf( void ){
	return( _serverConf );
}

std::string	Response::getServerName( void ){
	return( _serverName );
}

LocationConf	Response::getLocationConf( void ){
	return( _locationConf );
}

void Response::matchServerBlock(const std::vector<ServerConf>& serverConfs) {
	// match server block based on port
	for (size_t i = 0; i < serverConfs.size(); i++) {
		// server block we are currently inspecting

		// get ip ports map
		const std::map<std::string, std::set<int> > ipsPorts =  serverConfs[i].getIpPort();
		// loop through map
		for (std::map<std::string, std::set<int> >::const_iterator it = ipsPorts.begin(); it != ipsPorts.end(); it++) {
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
	:_request(reqObj), _redirectDest(""), _setCookieValue("") {

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

	_pathInfo = PathInfo( _serverConf.getRootDir() + _request.getUri() );
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

bool Response::isCgiRequest( void ) {

    // Check if URI matches CGI patterns
    if ( !_locationConf.getCgiPath().empty() && !_locationConf.getCgiExt().empty() )
    {
        // Strip query parameters by finding the first '?'
        std::string path = _request.getUri();
        size_t queryPos = path.find('?');
        if (queryPos != std::string::npos) {
            path = path.substr(0, queryPos);
        }

        // Find file extension in the path (without query parameters)
        size_t dot = path.rfind('.');
        if (dot != std::string::npos) {
            std::string ext = path.substr(dot);

            // Get allowed CGI extensions
            const std::vector<std::string>& cgiExts = _locationConf.getCgiExt();

            // Check if extension is allowed
            return (std::find(cgiExts.begin(), cgiExts.end(), ext) != cgiExts.end());
        }
    }
    return false;
}

bool	Response::isredirectRequest( void ){

	/*redirect request would override all request heirarchy
	for example we could perform a GET, POST or DELETE in an old folder 
	and that request would need to be redirected and passed on */

	if( !_locationConf.getAllowedRedirects().empty() || !_serverConf.getAllowedRedirects().empty() )
		return( true );

	return( false );
}

void		Response::processResponse( void ){

	//redirect request
	if( isredirectRequest() ){
		std::cout << "REDIRECT" << '\n';
		HandleRedirectRequest( _request );
	}

	//cgi request
	else if( isCgiRequest() ){
		std::cout << "CGI REQUEST" << '\n';
		::handleCgi( *this );
	}

	//get request
	else if( _request.getMethod() == "GET" ) {
		std::cout << "GET REQUEST" << '\n';
		HandleGetRequest();
	}
	//post request
	else if( _request.getMethod() == "POST" ) {
		std::cout << "POST REQUEST" << '\n';
		HandlePostRequest();
	}
	//delete request
	else if( _request.getMethod() == "DELETE" ) {
		std::cout << "DELETE REQUEST" << '\n';
		HandleDeleteRequest();
	}
}


void Response::setSetCookieValue(std::string value) {
	_setCookieValue = value;
}
