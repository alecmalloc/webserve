#ifndef ERROR_RESPONSE_HPP
#define ERROR_RESPONSE_HPP

class ErrorResponse {
    private:
        int                         _statusCode;
        std::vector<ServerConf>     _serverConfs;
        ServerConf                  _serverConf;
        HttpRequest                 _request;
        bool                        _isServerConf;
        std::string                 _httpResponse;
        std::string                 _headers;
        std::string                 _serverName;
        std::string                 _errorPage;
    public:
        ErrorResponse();
        ErrorResponse(const int& statusCode, const HttpRequest& request , const std::vector<ServerConf>& serverConfs);
        ErrorResponse(const ErrorResponse& other);
        ~ErrorResponse();
        
        const ErrorResponse& operator =(const ErrorResponse& other);

        // matching
        void matchServerBlock();
        void matchErrorPage();


        // lone get for checkEvents LOLOOL
        std::string getHttpResponse( void );

        void generateHttpResponse();
        std::string generateHeaders(std::string& body);
};

// helpers
std::string generateStatusLine(int statusCode);
std::string getCurrentDateTime();
std::string intToString(int number);
std::string generateReasonPhrase(int httpCode);

#endif
