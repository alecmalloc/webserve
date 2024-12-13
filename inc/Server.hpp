#ifndef SERVER_HPP
#define SERVER_HPP

class Server {
    private:
        int socket_fd;
    public:
        void start();
};

#endif