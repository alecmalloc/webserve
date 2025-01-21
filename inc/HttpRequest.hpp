#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include "Response.hpp"

class HttpRequest {
    private:
        // main member vars
        int                                                  _fd;
        Config&                                              _conf;
        Response                                             _response;
        // http request attributes
        std::string                                          _method;
        std::string                                          _uri;
        std::string                                          _url;
        std::string                                          _version;
        std::map<std::string, std::vector<std::string> >      _headers;
        std::string                                          _body;

        // reference to which serverConf i have matched from request
        ServerConf*                                         _server;

    public:
        HttpRequest(int fd, Config& conf);
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator =(const HttpRequest& other);
        ~HttpRequest() {};

        // getters
        int                                                     getFd() const;
        Config&                                                 getConf() const;
        const Response                                          getResponse() const;
        std::string                                             getUri() const;
        std::string                                             getMethod() const;
        std::string                                             getUrl() const;
        std::string                                             getVersion() const;
        std::map<std::string, std::vector<std::string> >         getHeaders() const;
        std::string                                             getBody() const;
        ServerConf*                                             getServer() const;

        void                                                    parse();
};

// << overload for printing
std::ostream& operator<<(std::ostream& os, HttpRequest& request);

#endif