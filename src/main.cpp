#include "../inc/Server.hpp"
// #include "../inc/Client.hpp"
#include "../inc/HttpHelpers.hpp"

#include <iostream>

int main(int argc, char**argv) {
    
    // assuming we need args to init server. or option to
    (void)argc;
    // (void)argv;
    std::string test = urlDecoder(argv[1]);
    std::cout << test << std::endl;

    // Server server;
    // server.start();
    return 0;
}