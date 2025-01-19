#include "webserv.hpp"

HttpRequest::HttpRequest(int fd, Config& conf): _fd(fd), _conf(conf) {
    ;
}

HttpRequest::HttpRequest(const HttpRequest& other): 
    _fd(other._fd), 
    _conf(other._conf),
    _response(other._response),
    _method(other._method),
    _uri(other._uri),
    _url(other._url),
    _version(other._version),
    _headers(other._headers),
    _body(other._body)
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

const Response HttpRequest::getResponse() const {
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

std::map<std::string, std::vector<std::string> > HttpRequest::getHeaders() const {
    return _headers;
}

std::string HttpRequest::getBody() const {
    return _body;
}

void HttpRequest::parse() {

    // read from fd and poll for fd to be ready
    char buffer[1024];
    ssize_t bytes_read;
    std::string request_data;
    // read out bytes from poll event_fd
    while ((bytes_read = read(_fd, buffer, sizeof(buffer) - 1)) > 0) {
        request_data += buffer;
    }
    // in case we didnt read anything return a server error
    if (bytes_read < 0 || request_data.empty()) {
        _response.setStatus(500);
        return;
    }

    // split request data into lines
    std::stringstream ss(request_data);
    std::string line;
    // make sure first line exists again (just to be sure)
    if (!std::getline(ss, line)) {
        _response.setStatus(400);
        return;
    }
    // parse request line (METHOD URI HTTP/VERSION)
    std::istringstream request_line(line);
    std::string method, uri, version;
    request_line >> method >> uri >> version;
    // check if method uri and version exist
    if (method.empty() || uri.empty() || version.empty()) {
        _response.setStatus(400);
        return;
    }
    // set ss to member variables
    _method = method;
    _uri = uri;
    _version = version;

    while (std::getline(ss, line) && !line.empty() && line != "\r") {
        if (line[line.length()-1] == '\r')
            line = line.substr(0, line.length()-1);
    
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 2); // skip ": "
            
            std::vector<std::string> values;
            
            // special handling for Cookie header
            if (key == "Cookie") {
                std::stringstream valueStream(value);
                std::string item;
                while (std::getline(valueStream, item, ';')) {
                    // trim leading/trailing whitespace
                    item.erase(0, item.find_first_not_of(" "));
                    item.erase(item.find_last_not_of(" ") + 1);
                    values.push_back(item);
                }
            } else {
                // normal comma-separated header handling
                std::stringstream valueStream(value);
                std::string item;
                while (std::getline(valueStream, item, ',')) {
                    item.erase(0, item.find_first_not_of(" "));
                    item.erase(item.find_last_not_of(" ") + 1);
                    values.push_back(item);
                }
            }
            
            _headers[key] = values;
        }
    }

    // get body if present (everything after empty line)
    std::string body;
    while (std::getline(ss, line)) {
        body += line + "\n";
    }
    if (!body.empty()) {
        _body = body;
    }
}
