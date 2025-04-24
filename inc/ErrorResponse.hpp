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
    public:
        ErrorResponse();
        ErrorResponse(const int& statusCode, const HttpRequest& request , const std::vector<ServerConf>& serverConfs);
        ErrorResponse(const ErrorResponse& other);
        ~ErrorResponse();
        
        const ErrorResponse& operator =(const ErrorResponse& other);

        // matching
        void matchServerBlock();
        void matchErrorPage();
        void matchServerName();

        void generateHttpResponse();
        void generateHeaders()
};






#endif
