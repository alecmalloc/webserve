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

class HttpRequest;


class Response {
    private:
        std::string 								_httpResponse;
		int 										_ResLen;
		std::string									_statusLine;
		std::map <std::string, std::string>			_headerMap;
		std::string									_body;
		long                                		_fileSize;
		std::string									_filename;
		std::string									_reasonPhrase;
		
		ServerConf									_serverConf;
		LocationConf 								_locationConf;
		HttpRequest									_request;
		// for redirects
		std::string									_redirectDest;

		// to make cookies work
		// cookie value we want to have
		std::string									_setCookieValue;

        int											_statusCode; // http status code
		std::string									_serverName;
		PathInfo									_pathInfo;

    public:
        ~Response();

		Response();
		Response(HttpRequest& reqObj, const std::vector<ServerConf>& serverConfs);
        void    processResponse(HttpRequest &ReqObj);
		void generateErrorResponse(HttpRequest &reqObj);
		void generateHeader();
		std::string generateStatusLine();
		std::string genarateReasonPhrase(int HttpCode);
		void generateHttpresponse();
		std::string intToString(int number);
		std::string serveFileContent(const PathInfo& pathInfo);
		bool isCgiRequest(const std::string& uri);
		void handlePostData(const HttpRequest& ReqObj);
		std::string generateDirectoryListing(const std::string& path);
		std::string uploadPathhandler(const LocationConf* locationConf);
		void handleMultipartUpload(HttpRequest& ReqObj, PathInfo& pathinfo, const std::string& boundary);

		int checkContentLength(HttpRequest& ReqObj);

		void HandleGetRequest(HttpRequest& ReqObj, PathInfo& pathInfo);
		//helpers for get req
		std::string constructFullPath(const std::string uri);
		bool serveFileIfExists(const std::string& fullPath, HttpRequest& ReqObj);
		bool serveRootIndexfile(HttpRequest& ReqObj, std::string fullPath);
		bool serveLocationIndex(HttpRequest& ReqObj);


		
		// Handlers
		void HandleRedirectRequest(HttpRequest& ReqObj, std::map<std::string, std::string > redirect);
		void HandleDeleteRequest(HttpRequest& ReqObj, PathInfo& pathInfo);
		void HandlePostRequest(HttpRequest& ReqObj,PathInfo& pathinfo);
		void genarateUploadSucces(HttpRequest& ReqObj);

        // Getters
		const std::map<int, std::string>& getErrorPages() const;
        std::string getHttpResponse() const { return _httpResponse; }
        int getResLen() const { return _ResLen; }
        std::string getStatusLine() const { return _statusLine; }
        std::map<std::string, std::string> getHeaderMap() const { return _headerMap; }
        std::string getBody() const { return _body; }
        long getFileSize() const { return _fileSize; }
        std::string getFilename() const { return _filename; }
        int getStatusCode() const { return _statusCode; }
		std::string getReasonPhrase() const;
		std::string getServerName();
		std::string getCurrentDateTime();
		std::string getContentType();
		std::string getRedirectDest();

        // Setters
        void setHttpResponse(const std::string& httpResponse) { _httpResponse = httpResponse; }
        void setResLen(int resLen) { _ResLen = resLen; }
        void setStatusLine(const std::string& statusLine) { _statusLine = statusLine; }
        void setHeaderMap(const std::map<std::string, std::string>& headerMap) { _headerMap = headerMap; }
        void setBody(const std::string& body) { _body = body; }
        void setFileSize(long fileSize) { _fileSize = fileSize; }
        void setFilename(const std::string& filename) { _filename = filename; }
        void setStatusCode(int statusCode) { _statusCode = statusCode; }
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
		void matchServerName( void )
};


#endif
