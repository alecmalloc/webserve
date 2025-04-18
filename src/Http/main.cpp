#include "webserv.hpp"

int main(int argc, char *argv[]) {

	std::string	confFile;	
	Config config;

	// check for config file
	if ( argc == 2 ) 
		confFile = argv[ 1 ];
	else if ( argc == 1 )	
		confFile = DEFAULT_CONF;	
	else {
		std::cerr << RED << \
			"Too many Arguments. Usage:\n" << "\033[0;5m"  << \
			"./webserv confFile" << END << std::endl;
		return( 1 );
	}

	// Parse Config File 
	try {
		config.parse( confFile );
	}
	catch ( std::runtime_error &e ){
		std::cerr <<  RED << 
			"ERROR: " << e.what() << END << std::endl;
		return( 1 );
	}

	//start server
	try {
		Server	server( config );
		server.runServer();
	}
	catch ( std::runtime_error &e ){
		std::cerr << RED << "ERROR: " << e.what() << END << std::endl;
		return( 1 );
	}

	return 0;
}
