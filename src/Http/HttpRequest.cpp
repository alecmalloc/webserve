#include "webserv.hpp"

//constructors
HttpRequest::HttpRequest( ): _response_code( 200 ), _port( -1 ) {
    _cgiResponseString = "";
}

HttpRequest::HttpRequest(const HttpRequest& other): _response_code(other._response_code), \
		_method(other._method), _uri(other._uri), _url(other._url), _version(other._version), \
		_headers(other._headers), _body(other._body), _hostname(other._hostname){
    ;
}

HttpRequest& HttpRequest::operator =( const HttpRequest& other ){
    if( this != &other ){
        _method = other.getMethod();
        _uri = other.getUri();
        _url = other.getUrl();
        _version = other.getVersion();
        _headers = other.getHeaders();
        _body = other.getBody();
        _hostname = other.getHostname();
        _response_code = other.getResponseCode();
    }
    return( *this );
}

HttpRequest::~HttpRequest( void ){;}

//getters
std::string	HttpRequest::getCgiResponseString() const {
    return( _cgiResponseString );
}

std::string	HttpRequest::getHostname() const {
    return( _hostname );
}

int		HttpRequest::getPort() const {
    return( _port );
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

std::map<std::string, std::vector<std::string> >	HttpRequest::getHeaders() const {
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

void	HttpRequest::setResponseCode( int tmp ){
	_response_code = tmp;
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

