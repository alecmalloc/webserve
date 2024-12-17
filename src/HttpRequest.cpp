#include "../inc/HttpRequest.hpp"

HttpRequest::HttpRequest(int fd): _fd(fd) {
    HttpError error = parse();

    if (error != 200) {
        std::stringstream output;
        output << error;
        throw std::runtime_error(output.str());
    }
}

HttpError HttpRequest::parse(void) {

    // read from fd and poll for fd to be ready (c stuff)
    char buffer[1024];
    ssize_t bytes_read;
    struct pollfd fds[1];
    fds[0].fd = _fd;
    fds[0].events = POLLIN;
    // 5 second time out
    int timeout = 5000;
    if (poll(fds, 1, timeout) > 0) {
        bytes_read = read(_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0)
            buffer[bytes_read] = '\0';
    }

    // traverse using an iss (cpp way)
    std::string line;
    std::istringstream iss(buffer);

    // get first line (METHOD, URI, VERSION)
    getline(iss, line);
    std::istringstream lineStream(line);
    if (!(lineStream >> _method >> _uri >> _version))
        return BAD_REQUEST;
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
        return METHOD_NOT_ALLOWED;
    // validate uri TODO
    if (_version.substr(5, 3) != "1.1")
        return HTTP_VERSION_NOT_SUPPORTED;
    
    // get rest of request
    while (getline(iss, line)) {
        // break for body
        if (line.empty())
            break;
        // find position of colon
        size_t colon = line.find(':');
        if (colon == std::string::npos)
            return BAD_REQUEST;
        // extract key and value
        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        std::cout << value << '\n';
        // TODO parse rest of value into vector 
    }

    // TODO handle body

    return OK;
}