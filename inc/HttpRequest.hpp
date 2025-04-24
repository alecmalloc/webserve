#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include "PathInfo.hpp"

class HttpRequest {
	private:
		// main member vars
		int							_response_code;
		// http request attributes
		std::string						_method;
		// uri: /articles/posts1.html
		std::string						_uri;
		// url: example.com/
		std::string						_url;
		std::string						_version;
		std::map<std::string, std::vector<std::string> >	_headers;
		std::string						_body;
		//cgi response string 
		std::string						_cgiResponseString;
		std::string						_hostname;
		int        						_port;

	public:
		HttpRequest();
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
		std::string                                             getCgiResponseString() const;
		int                                                     getPort() const;
		std::string						getConnectionType() const;

		//setters
		void	setMethod( std::string );
		void	setUri( std::string );
		void	setUrl( std::string );
		void	setVersion( std::string );
		void	setBody( std::string );
		void	setHeader( std::string, std::string);
		void	setResponseCode( int tmp );
		void    setCgiResponseString(const std::string& cgiResponseString);

    void    setPort(const int port);
    //set path in PathInfo after already initilized
    void	setPathInfoPath( std::string path );


    // void    handleRequest();

		// main handler
		void    handleRequest(const std::string& rawRequest);
		// void    handleRequest();

		// member funcitons
		void    parse(const std::string& rawRequest);
		void    parseHeaders(const std::string& rawRequest);
		void    parseBody(const std::string& rawRequest);
		void    parseBodyChunked(const std::string& rawRequest, size_t bodyStart);

};

// << overload for printing
std::ostream& operator<<(std::ostream& os, HttpRequest& request);

#endif
