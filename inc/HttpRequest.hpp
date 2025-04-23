#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include "PathInfo.hpp"

class HttpRequest {
    private:
        // main member vars
        Config&                                              _conf;
        int                                                  _response_code;
        // http request attributes
        std::string                                          _method;
        // uri: /articles/posts1.html
        std::string                                          _uri;
        // url: example.com/
        std::string                                          _url;
        std::string                                          _version;
        std::map<std::string, std::vector<std::string> >      _headers;
        std::string                                          _body;
		//cgi response string 
		std::string											_cgiResponseString;
        std::string                                          _hostname;
        int                                                  _port;

        // reference to which serverConf i have matched from request
        ServerConf                                          _server;

        // path variables
        PathInfo                                            _pathInfo;

    public:
        HttpRequest(Config& conf);
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator =(const HttpRequest& other);
        ~HttpRequest() {};

        // getters
        Config&                                                 getConf() const;
        int                                                     getResponseCode() const;
        std::string                                             getUri() const;
        std::string                                             getMethod() const;
        std::string                                             getUrl() const;
        std::string                                             getVersion() const;
        std::map<std::string, std::vector<std::string> >        getHeaders() const;
        std::string                                             getBody() const;
        std::string                                             getHostname() const;
        ServerConf						                        getServer() const;
        PathInfo                                                getPathInfo() const;
		std::string                                             getCgiResponseString() const;
        int                                                     getPort() const;

        //setters
        void	setMethod( std::string );
        void	setUri( std::string );
        void	setUrl( std::string );
        void	setVersion( std::string );
        void	setBody( std::string );
        void	setHeader( std::string, std::string);
        void	setConfig( Config& );
        void	setResponseCode( int tmp );
        void	setServer( ServerConf );
        void    setPathInfo( PathInfo );
		void    setCgiResponseString(const std::string& cgiResponseString);
        void    setPort(const int port);
	//set path in PathInfo after already initilized
	void	setPathInfoPath( std::string path );

        // main handler
        void    handleRequest(const std::string& rawRequest);
        // void    handleRequest();

	    // member funcitons
        void    parse(const std::string& rawRequest);
        void    parseHeaders(const std::string& rawRequest);
        void    parseBody(const std::string& rawRequest);
        void    parseBodyChunked(const std::string& rawRequest, size_t bodyStart);

        void    matchServerBlock(void);

		std::string getConnectionType() const;
};

// << overload for printing
std::ostream& operator<<(std::ostream& os, HttpRequest& request);

#endif
