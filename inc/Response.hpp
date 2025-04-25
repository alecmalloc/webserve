#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <ostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Cgi.hpp"

class HttpRequest;

class Response {
	private:
		//store pathInfo
		PathInfo				_pathInfo;
		//store response-> len, status, header, body
		std::string				_httpResponse;
		int					_ResLen;
		std::string				_statusLine;
		std::string				_body;
		//store file-> name, size
		long					_fileSize;
		std::string				_filename;

		std::string				_reasonPhrase;

		//store conf files
		ServerConf				_serverConf;
		LocationConf				_locationConf;

		//store request strinf
		HttpRequest				_request;

		// for redirects
		std::string				_redirectDest;

		// cookie value we want to have
		std::string				_setCookieValue;


		int					_statusCode;
		std::string				_serverName;
    public:

		//constructors
		Response();
		Response(HttpRequest& reqObj, const std::vector<ServerConf>& serverConfs);
		~Response();

		void    	processResponse();
		void		generateHeader();
		void		generateHttpResponse();
		std::string	serveFileContent();

		//cgi
		bool 		isCgiRequest( void );
		bool		isredirectRequest( void );
		
		void		handlePostData();
		std::string	uploadPathhandler( void );
		void		handleMultipartUpload( std::string contentType );
		void		checkContentLength( void );
		void		HandleGetRequest();
		std::string	constructFullPath();
		void		serveFileIfExists( const std::string& fullPath );
		void		serveRootIndexfile( void );
		void		serveLocationIndex();



		// Handlers
		void		HandleRedirectRequest( HttpRequest& ReqObj );
		void		HandleDeleteRequest();
		void		HandlePostRequest();
		void		genarateUploadSucces();

		// Getters
		const std::map<int, std::string>&	getErrorPages() const;
		std::string			getHttpResponse() const { return _httpResponse; }
		int				getResLen() const { return _ResLen; }
		std::string getStatusLine() const { return _statusLine; }
		std::string			getBody() const { return _body; }
		long				getFileSize() const { return _fileSize; }
		std::string			getFilename() const { return _filename; }
		int					getStatusCode() const { return _statusCode; }
		std::string			getReasonPhrase() const;
		std::string			getServerName();
		std::string			getRedirectDest();
		HttpRequest&			getHttpRequest( void );
		ServerConf			getServerConf( void );
		LocationConf			getLocationConf( void );

		// Setters
		void setHttpResponse(const std::string& httpResponse) { _httpResponse = httpResponse; }
		void setResLen(int resLen) { _ResLen = resLen; }
		void setStatusLine(const std::string& statusLine) { _statusLine = statusLine; }
		void setBody(const std::string& body) { _body = body; }
		void setFileSize(long fileSize) { _fileSize = fileSize; }
		void setFilename(const std::string& filename) { _filename = filename; }
		void setStatusCode(int statusCode) { statusCode = statusCode; }

		void setReasonPhrase(const std::string &reasonPhrase);
		void setRedirectDest(const std::string& redirectDest) { _redirectDest = redirectDest; };

		// for cookie setter
		void setSetCookieValue(std::string value);

		void createSession( void );
		void deleteSession( std::string sessionID );
		std::string findSession( std::vector<std::string> cookies );

		// alec functions
		// added these while refactoring
		void setBodyErrorPage(int httpCode);
		void handleCookiesPage(HttpRequest& request);

		void setStatus(int statusCode) {_statusCode = statusCode;};

		// matching functions
		void matchServerBlock(const std::vector<ServerConf>& serverConfs);
		void matchLocationConf( void );
};

// helpers
std::string	generateStatusLine( int statusCode );
std::string getCurrentDateTime();
std::string getContentType( HttpRequest& request, PathInfo& pathInfo );
std::string generateDirectoryListing(const std::string& path);
std::string intToString(int number);
std::string generateReasonPhrase(int httpCode);
std::string matchServerName( HttpRequest& request, ServerConf& serverConf );

#endif
