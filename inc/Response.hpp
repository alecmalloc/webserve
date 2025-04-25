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
		std::string				_statusLine;
		std::string				_body;

		//store file-> name, size
		long					_fileSize;
		std::string				_filename;
		
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
		bool					_isLocation;
    public:

		//constructors
		Response();
		Response(HttpRequest& reqObj, const std::vector<ServerConf>& serverConfs);
		~Response();

		// Getters
		const std::map<int, std::string>&	getErrorPages( void ) const;
		std::string				getHttpResponse( void ) const;
		std::string 				getStatusLine( void ) const;
		std::string				getBody( void ) const;
		std::string				getFilename( void ) const;
		std::string				getServerName( void );
		HttpRequest&				getHttpRequest( void );
		ServerConf				getServerConf( void );
		LocationConf				getLocationConf( void );
		bool					getIsLocation( void );

		// Setters
		void		setHttpResponse(const std::string& httpResponse) { _httpResponse = httpResponse; }
		void 		setResLen(int resLen) { _ResLen = resLen; }
		void 		setStatusLine(const std::string& statusLine) { _statusLine = statusLine; }
		void 		setBody(const std::string& body) { _body = body; }
		void 		setFileSize(long fileSize) { _fileSize = fileSize; }
		void 		setFilename(const std::string& filename) { _filename = filename; }
		void 		setStatusCode(int statusCode) { statusCode = statusCode; }
		void 		setRedirectDest(const std::string& redirectDest) { _redirectDest = redirectDest; };
		void 		setSetCookieValue(std::string value);
		void 		createSession( void );
		void		deleteSession( std::string sessionID );
		std::string findSession( std::vector<std::string> cookies );
		void		setBodyErrorPage(int httpCode);
		void		setStatus(int statusCode) {_statusCode = statusCode;};


		//memberfunctions
		void    	processResponse();

		// Handlers
		void		handleRedirectRequest( HttpRequest& ReqObj );
		void		handleDeleteRequest();
		void		handleGetRequest();
		void		handlePostRequest();
		void		handleCookiesPage(HttpRequest& request);

		// matching functions
		void		matchServerBlock(const std::vector<ServerConf>& serverConfs);
		void 		matchLocationConf( void );
};

// helpers
std::string	generateStatusLine( int statusCode );
std::string	getCurrentDateTime();
std::string 	getContentType( HttpRequest& request, PathInfo& pathInfo );
std::string 	generateDirectoryListing(const std::string& path);
std::string 	intToString(int number);
std::string 	generateReasonPhrase(int httpCode);
std::string 	matchServerName( HttpRequest& request, ServerConf& serverConf );

#endif
