#include "../inc/HttpRequest.hpp"
#include "../inc/StrUtils.hpp"
#include "../inc/Config.hpp"

HttpRequest::HttpRequest(int fd, Config& conf): _fd(fd), _conf(conf) {
    ;
}

HttpRequest::HttpRequest(const HttpRequest& other) {
    
}

HttpRequest& HttpRequest::operator =(const HttpRequest& other) {

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
        // return SERVER_ERROR;
    }

    if (request_data.empty()) {
        // return BAD_REQUEST;
    }

    // traverse using an iss (cpp way)
    std::string line;
    std::istringstream iss(buffer);

    // get first line (METHOD, URI, VERSION)
    getline(iss, line);
    std::istringstream lineStream(line);
    if (!(lineStream >> _method >> _uri >> _version))
        return BAD_REQUEST;
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
        return METHOD_NOT_ALLOWED;
    // validate uri TODO
    if (_version.substr(5, 3) != "1.1")
        return HTTP_VERSION_NOT_SUPPORTED;

    // get rest of request
    while (getline(iss, line)) {
        // break for body
        if (line.empty())
            break;
        // find position of colon
        size_t colon = line.find(':');
        if (colon == std::string::npos)
            return BAD_REQUEST;
        // extract key and value
        std::string key = ft_trim(line.substr(0, colon));
        std::string value = ft_trim(line.substr(colon + 1));
        _headers.insert(std::make_pair(key, value));
    }

    // add body to http request
    std::map<std::string, std::string>::iterator contentLenIt;
    contentLenIt = _headers.find("Content-Length");
    // if content-length is provided there is a body
    // store each line of body vector to
    if (contentLenIt != _headers.end()) {
        while(getline(iss, line))
            _body.push_back(ft_trim(line));
    }
}

const Response& HttpRequest::getResponse() {
    return _response;
}
