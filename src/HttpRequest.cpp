#include "../inc/HttpRequest.hpp"

HttpRequest::HttpRequest(int fd): _fd(fd) {
    HttpError errors = parse();

    if (errors.status > 0)
        throw std::runtime_error(errors.error);
}

HttpError HttpRequest::parse(void) {
    HttpError errors;

    // read file and poll for fd to be ready
    char buffer[1024];
    ssize_t bytes_read;
    struct pollfd fds[1];
    fds[0].fd = _fd;
    fds[0].events = POLLIN;
    // 5 second time out
    int timeout = 5000;
    if (poll(fds, 1, timeout) > 0) {
        bytes_read = read(_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::cout << buffer << '\n';
        }
    }

    return errors;
}