#include "webserv.hpp"

ErrorResponse::ErrorResponse() {

}

ErrorResponse::ErrorResponse(const ErrorResponse& other) {
    _serverConfs = other._serverConfs;
}

ErrorResponse::~ErrorResponse() {}

const ErrorResponse& ErrorResponse::operator =(const ErrorResponse& other) {
    if (this != &other) {
        _serverConfs = other._serverConfs;
    }
    return *this;
}