#include "webserv.hpp"

int main(int argc, char**argv) {
    
    // assuming we need args to init server. or option to
    if (argc != 2) {
        std::cerr << "Error: too few args to run" << std::endl;
        return -1;
    }
        

    // try to init Config with filename. runtime exception if no 
    try {
        Config config(argv[1]);
    }
    catch (std::runtime_error &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Server server;
    // server.start();
    return 0;
}
