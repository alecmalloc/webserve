#ifndef RESPONSE_HPP
#define RESPONSE_HPP

// PLACEHOLDER: using this until linus creates spec for response type
class Response {
    private:
        int m_statusCode; // http status code
    public:
        Response(): m_statusCode(200) {};
        ~Response() {};

        void setStatus(int statusCode) {m_statusCode = statusCode;};
};


#endif