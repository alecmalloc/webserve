#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include "PathInfo.hpp"

class HttpRequest {
	private:
		std::string						_method;
		std::string						_uri;
		std::string						_url;
		std::string						_version;
		std::map<std::string, std::vector<std::string> >	_headers;
		std::string						_body;
		std::string						_cgiResponseString;
		std::string						_hostname;
		int        						_port;
		int							_response_code;

	public:
		//constructors
		HttpRequest();
		HttpRequest(Config& conf);
		HttpRequest(const HttpRequest& other);
		~HttpRequest();

		//operator overloads
		HttpRequest& operator =(const HttpRequest& other);

		// getters
		Config&						getConf() const;
		std::string					getUri() const;
		std::string					getMethod() const;
		std::string					getUrl() const;
		std::string					getVersion() const;
		std::map<std::string, std::vector<std::string>>	getHeaders() const;
		std::string					getBody() const;
		std::string					getHostname() const;
		std::string					getCgiResponseString() const;
		int        					getPort() const;
		int        					getResponseCode() const;
		std::string					getConnectionType() const;

		//setters
		void	setMethod( std::string );
		void	setUri( std::string );
		void	setUrl( std::string );
		void	setVersion( std::string );
		void	setBody( std::string );
		void	setHeader( std::string, std::string);
		void	setResponseCode( int );
		void	setCgiResponseString( const std::string&  );
		void	setPort(const int );
		void	setPathInfoPath( std::string );

		// member funcitons
		void    handleRequest(const std::string& );
		void    parse(const std::string& );
		void    parseHeaders(const std::string& );
		void    parseBody(const std::string& );
		void    parseBodyChunked(const std::string& , size_t );

};

// << overload for printing
std::ostream& operator<<(std::ostream& os, HttpRequest& request);

#endif
