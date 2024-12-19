#include "Config.hpp"
#include "webserv.hpp"

void	Location_print( Location& location ){
	std::cout << location.getPath() << "\n" \
			<< location.getAllowedMethods() << "\n" << \
			location.getAllowedRedirects() << "\n" << \
			location.getAutoIndex() << "\n" << \
			location.getIndex() << "\n" << \
			location.getCgi() << "\n" << \
			location.getUploadDir() << "\n" << \
			location.getRootDir() << "\n" << std::endl;
}

void	Server_print( Server& server ){
	std::vector< Location >		location = server.getLocations();

	std::cout << server.getIpPort() << "\n" \
			<< server.getServerNames() << "\n" << \
			server.getErrorPages() << "\n" << \
			server.getBodySize() << "\n" << std::endl;
	for( std::vector< Location >::iterator it = location.begin(); it != location.end(); it++ ){
		Location_print( it.base() );	
	}

}

void	Config_print( Config& conf ){
	std::vector< Server >	server = conf.getServers();

	for( std::vector< Server >::iterator it = server.begin(); it != server.end(); it++ ){
		Server_print( it.base() );	
	}
}

int main(int argc, char**argv) {
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
	}

	// Parse Config File 
	try {
		config.parse( confFile );
	}
	catch (std::runtime_error &e) {
	    std::cerr <<  "\033[0;31m" << \
		 "Error: " << e.what() << "\033[0m" << std::endl;
	}
	
	// Server server;
	// server.start();
	return 0;
}
