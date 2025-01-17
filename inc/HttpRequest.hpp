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
        std::string                                          _url;
        std::string                                          _version;
        std::map<std::string, std::vector<std::string>>      _headers;
        std::string                                          _body;

        // other member vars
        int                                                  _fd;
        Config&                                              _conf;
        Response                                             _response;

        // TODO reference to which server/ server block i have matched
        // const ServerConf&                                   _server;

    public:
        HttpRequest(int fd, Config& conf);
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator =(const HttpRequest& other);
        ~HttpRequest() {};

        int                                                     getFd() const;
        Config&                                                 getConf() const;
        Response                                                getResponse() const;
        std::string                                             getUri() const;
        std::string                                             getMethod() const;
        std::string                                             getUrl() const;
        std::string                                             getVersion() const;
        std::map<std::string, std::vector<std::string>>         getHeaders() const;
        std::string                                             getBody() const;
        // todo getServer

        void                                                    parse();
        const Response&                                         getResponse();
};

#endif