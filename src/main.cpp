#include "../inc/Server.hpp"
#include "../inc/Client.hpp"
#include "../inc/Config.hpp"

#include <iostream>

int main(int argc, char**argv) {
    
    // assuming we need args to init server. or option to
    (void)argc;
    (void)argv;

    // try to init Config with filename. runtime exception if no 
    try {
        Config(argv[1]);
    }
    catch (std::runtime_error &e) {
        std::cerr << "Error:" << e.what() << std::endl;
    }

    // Server server;
    // server.start();
    return 0;
}