#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <iostream>
#include <map>

// c functions
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h> 

class HttpError {
    public:
        int         status;
        std::string error;
        HttpError() : status(0), error("") {};
};

enum HttpMethod {
    GET,
    POST,
    DELETE
};

class HttpRequest {
    private:
        int                                 _fd;
        // HttpMethod                          _method;
        std::string                         _uri;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
    public:
        HttpRequest() {};
        HttpRequest(int fd);
        ~HttpRequest() {};

        HttpError parse(void);
};

#endif