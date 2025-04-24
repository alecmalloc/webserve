#include "webserv.hpp"

//constructors
HttpRequest::HttpRequest()
{
	;
}

HttpRequest::HttpRequest( Config& conf ): _conf( conf ), _response_code( 200 ), _port( -1 ), _pathInfo() {
    _cgiResponseString = "";
}

HttpRequest::HttpRequest(const HttpRequest& other): _conf(other._conf), _response_code(other._response_code), \
		_method(other._method), _uri(other._uri), _url(other._url), _version(other._version), \
		_headers(other._headers), _body(other._body), _hostname(other._hostname), _server(other._server), \
		_pathInfo(other._pathInfo) {
    ;
}

HttpRequest& HttpRequest::operator =( const HttpRequest& other ){
    if( this != &other ){
        _conf = other.getConf();
        _method = other.getMethod();
        _uri = other.getUri();
        _url = other.getUrl();
        _version = other.getVersion();
        _headers = other.getHeaders();
        _body = other.getBody();
        _hostname = other.getHostname();
        _response_code = other.getResponseCode();
        _server = other.getServer();
        _pathInfo = other.getPathInfo();
    }
    return( *this );
}

HtttpRequest::~HttpRequest( void ){;}

//getters
std::string	HttpRequest::getCgiResponseString() const {
    return( _cgiResponseString );
}


ServerConf	HttpRequest::getServer() const {
    return( _server );
}

std::string	HttpRequest::getHostname() const {
    return( _hostname );
}

int		HttpRequest::getPort() const {
    return( _port );
}

Config&		HttpRequest::getConf() const {
    return( _conf );
}

int		HttpRequest::getResponseCode() const {
    return( _response_code );
}

std::string	HttpRequest::getUri() const {
    return( _uri );
}

std::string	HttpRequest::getUrl() const {
    return( _url );
}

std::string	HttpRequest::getVersion() const {
    return( _version );
}

std::string	HttpRequest::getMethod() const {
    return( _method );
}

PathInfo	HttpRequest::getPathInfo() const {
    return( _pathInfo );
}

std::map<std::string, std::vector<std::string>>	HttpRequest::getHeaders() const {
    return( _headers );
}

std::string	HttpRequest::getBody() const {
    return( _body );
}


//setters
void	HttpRequest::setCgiResponseString(const std::string& cgiResponseString) {
    _cgiResponseString = cgiResponseString;
}

void	HttpRequest::setMethod( std::string tmp ){
	_method = tmp;
}

void	HttpRequest::setUri( std::string tmp ){
	_uri = tmp;
}

void	HttpRequest::setUrl( std::string tmp ){
	_url = tmp;
}

void	HttpRequest::setVersion( std::string tmp ){
	_version = tmp;

}

void	HttpRequest::setBody( std::string tmp ){
	_body = tmp;
}

void	HttpRequest::setHeader( std::string tmp1, std::string tmp2 ){
	_headers[ tmp1 ].push_back( tmp2 );
}

void	HttpRequest::setConfig( Config& tmp ){
	_conf = tmp;
}

void	HttpRequest::setResponseCode( int tmp ){
	_response_code = tmp;
}

void	HttpRequest::setServer( ServerConf tmp ){
	_server = tmp;
}

void    HttpRequest::setPathInfo( PathInfo tmp ) {
    _pathInfo = tmp;
}

void	HttpRequest::setPathInfoPath( std::string path ){
	_pathInfo.setFullPath( path );
}


void HttpRequest::setPort(int port) {
    _port = port;
}

// overload for printing
std::ostream& operator<<(std::ostream& os, HttpRequest& request) {
    os << "Version: " << request.getVersion() << "\n";
    os << "Method: " << request.getMethod() << "\n";
    os << "Uri: " << request.getUri() << "\n";
    os << "Url: " << request.getUrl() << "\n";
    os << "Headers: \n";
    const std::map<std::string, std::vector<std::string> >& headers = request.getHeaders();
    for (std::map<std::string, std::vector<std::string> >::const_iterator header = headers.begin(); header != headers.end(); ++header) {
        os << "  " << header->first << ": ";
        for (std::vector<std::string>::const_iterator value = header->second.begin(); value != header->second.end(); ++value) {
            os << *value << " ";
        }
        os << "\n";
    }
    os << "Body: " << request.getBody() << "\n";
    return os;
}

