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

        int m_statusCode; // http status code
    public:
       // Response(): m_statusCode(200) {};
        ~Response() {};



        Response(void);
        Response(HttpRequest& reqObj);
        void    processResponse(HttpRequest &ReqObj);
		void generateErrorResponse(HttpRequest &reqObj);
		void generateHeader(HttpRequest &reqObj);
		void	generateStatusLine(HttpRequest &reqObj);

		std::string intToString(int number);

        // Getters
        std::string getHttpResponse() const { return _httpResponse; }
        int getResLen() const { return _ResLen; }
        std::string getStatusLine() const { return _statusLine; }
        std::map<std::string, std::string> getHeaderMap() const { return _headerMap; }
        std::string getBody() const { return _body; }
        long getFileSize() const { return _fileSize; }
        std::string getFilename() const { return _filename; }
        int getStatusCode() const { return m_statusCode; }

        // Setters
        void setHttpResponse(const std::string& httpResponse) { _httpResponse = httpResponse; }
        void setResLen(int resLen) { _ResLen = resLen; }
        void setStatusLine(const std::string& statusLine) { _statusLine = statusLine; }
        void setHeaderMap(const std::map<std::string, std::string>& headerMap) { _headerMap = headerMap; }
        void setBody(const std::string& body) { _body = body; }
        void setFileSize(long fileSize) { _fileSize = fileSize; }
        void setFilename(const std::string& filename) { _filename = filename; }
        void setStatusCode(int statusCode) { m_statusCode = statusCode; }



        void setStatus(int statusCode) {m_statusCode = statusCode;};
};


#endif
