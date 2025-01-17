#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

// c functions
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

enum HttpError {
    OK = 200,
    BAD_REQUEST = 400, // malformed request syntax, invalid request message framing
    METHOD_NOT_ALLOWED = 405, // method is not among GET, POST, or DELETE
    URI_TOO_LONG = 414, // uri exceeds server limits
    HEADER_FIELDS_TOO_LARGE = 431, // header fields are too large
    HTTP_VERSION_NOT_SUPPORTED = 505, // version is not HTTP/1.1
    PAYLOAD_TOO_LARGE = 413, // request body exceeds configured client_body_size
    LENGTH_REQUIRED = 411, // content-Length header is missing for POST requests
    NOT_FOUND = 404, // requested resource doesn't exist
    FORBIDDEN = 403, // access to resource is forbidden
    UNAUTHORIZED = 401 // authentication is required
};

class HttpRequest {
    private:
    int                                     _fd;
    std::string                             _method;
    std::string                             _uri;
    std::string                             _version;
    std::map<std::string, std::string>      _headers;
    std::vector<std::string>                _body;
    public:
        HttpRequest() {};
        HttpRequest(int fd);
        ~HttpRequest() {};

        HttpError parse(void);
};

#endif