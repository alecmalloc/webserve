#include "webserv.hpp"

int main(int argc, char *argv[]) {
	std::string	confFile;	
       	Config config;

	// check for config file
	if ( argc == 2 ) 
		confFile = argv[ 1 ];
	else if ( argc == 1 )	
		confFile = DEFAULT_CONF;	
	else{
		std::cerr << "\033[0;31m" << \
			"Too much Arguments. Usage:\n" << "\033[0;5m"  << \
			"./webserv confFile" << "\033[0m" << std::endl;
		return( 1 );
	}

	// Parse Config File 
	try {
		config.parse( confFile );
	}
	catch (std::runtime_error &e) {
	    std::cerr <<  "\033[0;31m" << \
		 "Error: " << e.what() << "\033[0m" << std::endl;
	}
	std::vector< ServerConf > tmp = config.getServerConfs();
	for( std::vector< ServerConf >::const_iterator it = tmp.begin(); it != tmp.end(); it++ ){
		std::cout << *it << std::endl;
	}
  
   // *** TESTING FOR HTTP_REQUEST
    // #include <fcntl.h>
    // #include <unistd.h>
    // int fd = open("get.example", O_RDONLY);
    // if (fd < 0)
    //     return -1;
    // try {
    //     HttpRequest http(fd);
    // }
    // catch (std::runtime_error &e) {
    //     std::cerr << "Error: " << e.what() << std::endl;
    // }
  
	return 0;
}
