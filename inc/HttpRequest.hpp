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

#include "../inc/Response.hpp"
#include "../inc/Server.hpp"

class HttpRequest {
    private:
        // request attributes
        std::string                                          _method;
        std::string                                          _uri;
        std::string                                          _version;
        std::map<std::string, std::vector<std::string>>      _headers;
        std::string                                          _body;

        // other member vars
        const int                                            _fd;
        const Config&                                        _conf;
        Response                                             _response;

        // TODO reference to which server/ server block i have matched
        // const ServerConf&                                   _server;

    public:
        HttpRequest(int fd, Config& conf);
        ~HttpRequest() {};

        void parse();
        const Response& getResponse();
};

#endif