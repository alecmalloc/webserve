#include "Config.hpp"
#include "webserv.hpp"
#include <string>

template< typename T >
static void	print_Vec( const std::vector< T >& vec ){
	for( typename std::vector<T>::const_iterator it = vec.begin(); \
		it != vec.end(); it++ ){
		std::cout << *it << " ";
	}
	std::cout << std::endl;
}	

template< typename K, typename V  >
static void	print_Map( const std::map< K, V >& vec ){
	for( typename std::map< K, V >::const_iterator it = vec.begin(); \
		it != vec.end(); it++ ){
		std::cout << it->first << ":" << it->second <<  " ";
	}
	std::cout << std::endl;
}	

void	Location_print( Location& location ){
	std::cout << "Path: " << location.getPath() << "\n" \
		<< "Upload Dir: " << location.getUploadDir() << "\n" << \
		" Root Dir: " << location.getRootDir() << "\n" << std::endl;
	std::cout << "Allowed Methods: ";
	print_Vec( location.getAllowedMethods() );
	std::cout << "Allowed Redirects: ";
	print_Map( location.getAllowedRedirects() );
	std::cout << "Auto Index: ";
	if ( location.getAutoIndex() )
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
	std::cout << "Indeces: ";
	print_Vec( location.getIndex() );
	std::cout << "Cgi: ";
	print_Map( location.getCgi() );
}

void	Server_print( Server& server ){
	std::vector< Location >		location = server.getLocations();

	std::cout << "Ip Port: " << server.getIpPort() << "\n" \
		<< "Max Body Size: " << server.getBodySize() << "\n" << std::endl;
	std::cout << "Server Names: ";
	print_Vec( server.getServerNames() );
	std::cout << "Error Pages: ";
	print_Map( server.getErrorPages() );
	for( std::vector< Location >::iterator it = location.begin(); it != location.end(); it++ ){
		Location_print( *it );	
	}

}

void	Config_print( Config& conf ){
	std::vector< Server >	server = conf.getServers();

	for( std::vector< Server >::iterator it = server.begin(); it != server.end(); it++ ){
		Server_print( *it );	
	}
}

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
	
	// Server server;
	// server.start();
	return 0;
}
