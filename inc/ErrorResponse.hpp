#ifndef ERROR_RESPONSE_HPP
#define ERROR_RESPONSE_HPP




class ErrorResponse {
    private:
        std::vector<ServerConf>     _serverConfs;
    public:
        ErrorResponse();
        ErrorResponse(const std::vector<ServerConf>& serverConfs);
        ErrorResponse(const ErrorResponse& other);
        ~ErrorResponse();
        
        const ErrorResponse& operator =(const ErrorResponse& other);

        void    setServerConf(std::vector<ServerConf>& serverConfs) { _serverConfs = serverConfs ;};

        std::vector<ServerConf>    getServerConf( void ) { return _serverConfs ;};
};







#endif