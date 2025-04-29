//#include "../inc/Response.hpp"
#include "webserv.hpp"
#include <sstream>

//constructors
Response::Response()
	: _redirectDest(""), _setCookieValue(""), _isLocation( false ) {
		;
}

Response::Response( HttpRequest& reqObj, const std::vector<ServerConf>& serverConfs )
	: _request( reqObj ), _redirectDest( "" ), _setCookieValue( "" ), _isLocation( false ) {

	// check if request flagged as non 200 val
	if (! ( reqObj.getResponseCode() >= 200 && reqObj.getResponseCode() <= 302 ) )
		throw( reqObj.getResponseCode() );
	
	// set status code to state from req obj
	setStatus( reqObj.getResponseCode() );

	// match and set server block
	matchServerBlock( serverConfs );

	// match server name if exists
	_serverName = matchServerName( _request, _serverConf );

	// match and set location block
	matchLocationConf();

	//check allowed methods
	checkMethods();

	_pathInfo = PathInfo( _serverConf.getRootDir() + _request.getUri() );
}

Response::~Response(){
	;
}

//getters

HttpRequest&		Response::getHttpRequest( void ){
	return( _request );
}

ServerConf		Response::getServerConf( void ){
	return( _serverConf );
}

std::string		Response::getServerName( void ){
	return( _serverName );
}

LocationConf		Response::getLocationConf( void ){
	return( _locationConf );
}

bool			Response::getIsLocation( void ){
	return( _isLocation );
}

std::string		Response::getHttpResponse( void ) const{
	return( _httpResponse );
}

std::string		Response::getStatusLine( void ) const {
	return( _statusLine );
}

std::string		Response::getBody( void ) const {
	return( _body );
}

std::string		Response::getFilename( void ) const {
	return( _filename );
}

//setters

void			Response::setHttpResponse( const std::string& httpResponse ){ 
	_httpResponse = httpResponse;
}

void 			Response::setBody( const std::string& body ){ 
	_body = body; 
}

void 			Response::setFilename( const std::string& filename ){
       	_filename = filename;
}

void 			Response::setStatus( int statusCode ){ 
	_statusCode = statusCode;
}

void 			Response::setRedirectDest( const std::string& redirectDest ){ 
	_redirectDest = redirectDest; 
}

/*void	Response::setReasonPhrase( const std::string &reasonPhrase ) {
	_reasonPhrase = reasonPhrase;
}*/

void	Response::setBodyErrorPage( int httpCode ){

	setBody("<html><body><h1>"
	+ ::intToString(httpCode)
	+ " "
	+ ::generateReasonPhrase(httpCode)
	+ "Error "
	+ ::intToString(httpCode)
	+ "</h1></body></html>\n");
}

void Response::setSetCookieValue(std::string value) {
	_setCookieValue = value;
}

//helper functions

void	Response::checkMethods( void ){

	if( !_isLocation )
		return;
	std::vector< std::string >	tmp = _locationConf.getAllowedMethods();

	for( size_t i = 0; i < tmp.size(); i++ )
		if( tmp[i] == _request.getMethod() )
			return;
	
	throw( 405 );
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
				return;
			}
		}
	}
	throw( 404 );
}

void Response::matchLocationConf(void) {
	const std::vector<LocationConf>& locations = _serverConf.getLocationConfs();

	std::string uri = _request.getUri();

	if( uri.empty() || uri == "/customCookiesEndpoint/CookiesPage" \
	|| uri == "/customCookiesEndpoint/CookiesPage/activate" || uri == "/customCookiesEndpoint/CookiesPage/deactivate" )
		return;
	// Find best matching location (longest prefix match)
	std::string bestMatch = "";

	for ( size_t i = 0; i < locations.size(); i++ ) {
		std::string locPath = locations[i].getPath();

		if( locPath == "/" && locPath == uri ){
			bestMatch = locPath;
			_locationConf = _serverConf.getLocationConfs()[i];
			_isLocation = true;
		}
		else if( uri.find(locPath) == 0 && locPath.size() > 1 ){
			if( locPath.length() > bestMatch.length() ){
				bestMatch = locPath;
				_locationConf = _serverConf.getLocationConfs()[i];
				_isLocation = true;
			}
		}
	}
	if( !_isLocation )
		throw( 404 );
}

void	Response::generateHttpResponse( void ){

	std::stringstream header;
	header << ::generateStatusLine(_statusCode) << "\r\n";
	header << "Date: " << getCurrentDateTime() << "\r\n";
	header << "Server: " << _serverName << "\r\n";
	header << "Content-Type: " << ::getContentType(_request, _pathInfo) << "\r\n";
	header << "Content-Length: " << ::intToString(getBody().length()) << "\r\n";
	header << "Connection: " << _request.getConnectionType() << "\r\n";

	if (!(_setCookieValue.empty())) {
		header << "Set-Cookie: " << _setCookieValue << "\r\n";
	}

	// to set location (esp for redirects)
	if ( (!(_redirectDest.empty())) && (_statusCode == 301 || _statusCode == 302)) {
		header << "Location: " << _redirectDest << "\r\n";
	}

	header << "\r\n"; // End of headers
	std::string responseString = header.str() + getBody();
	setHttpResponse(responseString);
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
		//std::cout << "REDIRECT" << '\n';
		handleRedirectRequest();
	}

	//cgi request
	else if( isCgiRequest() ){
		//std::cout << "CGI REQUEST" << '\n';
		::handleCgi( *this );
	}

	//get request
	else if( _request.getMethod() == "GET" ) {
		//std::cout << "GET REQUEST" << '\n';
		handleGetRequest();
	}
	//post request
	else if( _request.getMethod() == "POST" ) {
		//std::cout << "POST REQUEST" << '\n';
		handlePostRequest();
	}
	//delete request
	else if( _request.getMethod() == "DELETE" ) {
		//std::cout << "DELETE REQUEST" << '\n';
		handleDeleteRequest();
	}
}
