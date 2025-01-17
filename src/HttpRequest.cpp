#include "../inc/HttpRequest.hpp"
#include "../inc/StrUtils.hpp"
#include "../inc/Config.hpp"

HttpRequest::HttpRequest(int fd, Config& conf): _fd(fd), _conf(conf) {
    ;
}

HttpRequest::HttpRequest(const HttpRequest& other): 
    _fd(other._fd), 
    _conf(other._conf),
    _method(other._method),
    _uri(other._uri),
    _url(other._url),
    _version(other._version),
    _headers(other._headers),
    _body(other._body),
    _response(other._response)
{
    ;
}

HttpRequest& HttpRequest::operator =(const HttpRequest& other) {
    if (this != &other) {
        _fd = other.getFd();
        _conf = other.getConf();
        _method = other.getMethod();
        _uri = other.getUri();
        _url = other.getUrl();
        _version = other.getVersion();
        _headers = other.getHeaders();
        _body = other.getBody();
        _response = other.getResponse();
    }

    return (*this);
}

int HttpRequest::getFd() const {
    return _fd;
}

Config& HttpRequest::getConf() const {
    return _conf;
}

Response HttpRequest::getResponse() const {
    return _response;
}

std::string HttpRequest::getUri() const {
    return _uri;
}

std::string HttpRequest::getUrl() const {
    return _url;
}

std::string HttpRequest::getVersion() const {
    return _version;
}

std::string HttpRequest::getMethod() const {
    return _method;
}

std::map<std::string, std::vector<std::string>> HttpRequest::getHeaders() const {
    return _headers;
}

std::string HttpRequest::getBody() const {
    return _body;
}

void HttpRequest::parse() {
    
    // read from fd and poll for fd to be ready (c stuff)
    char buffer[1024];
    ssize_t bytes_read;
    std::string request_data;

    while ((bytes_read = read(_fd, buffer, sizeof(buffer) - 1)) > 0) {
        request_data += buffer;
    }

    if (bytes_read < 0 || request_data.empty()) {
        _response.setStatus(500);
        return;
    }

    if (request_data.empty()) {
        _response.setStatus(400);
        return;
    }

    // traverse using an iss
    std::string line;
    std::istringstream iss(buffer);

   
}
