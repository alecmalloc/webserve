#include "Server.hpp"
#include <ostream>
#include <string>

//constructors

Server::Server( void ) : _bodySize( 0 ) {;}

Server::Server( const Server& og ){
	_ipPort = og._ipPort;
	_serverNames = og._serverNames;
	_locations = og._locations;
	_errorPages = og._errorPages;
	_bodySize = og._bodySize;
}

Server::~Server( void ) {;}

//operator overloads

Server&		Server::operator =( const Server& og ) {
	if ( this != &og ){
		_ipPort = og._ipPort;
		_serverNames = og._serverNames;
		_locations = og._locations;
		_errorPages = og._errorPages;
		_bodySize = og._bodySize;
	}
	return( *this );
}

std::ostream&	operator <<( std::ostream& os, const Server& server) {

	os << "Server Details:\n" << std::endl;

	// Print IP and Port
	if ( !server.getIpPort().empty() )
		os << "IP and Port: " << server.getIpPort() << "\n";

	// Print Server Names
	if ( !server.getServerNames().empty() ){
		os << "Server Names: ";
		std::vector< std::string > tmp = server.getServerNames();
		for (std::vector<std::string>::const_iterator it = tmp.begin(); 
				it != tmp.end(); ++it) {
			os << *it << " ";
		}
		os << "\n";
	}

	// Print Error Pages
	if ( !server.getErrorPages().empty() ){
		os << "Error Pages:\n";
		std::map< int, std::string > tmp2 = server.getErrorPages();
		for (std::map<int, std::string>::const_iterator it = tmp2.begin(); 
				it != tmp2.end(); ++it) {
			os << "Error " << it->first << ": " << it->second << "\n";
		}
	}

	// Print Body Size Limit
	if ( server.getBodySize() != 0 ){
		os << "Body Size Limit: " << server.getBodySize() << " bytes\n";

	}

	// Print Locations
	if ( !server.getLocations().empty() ){
		os << "Locations:\n";
		std::vector< Location >	tmp3 = server.getLocations();
		for (std::vector<Location>::const_iterator it = tmp3.begin(); 
				it != tmp3.end(); ++it) {
			os << *it << "\n";
		}
	}
	
	return( os );
}


//getter functions
const std::string	Server::getIpPort( void ) const {
	return( _ipPort );
}

const std::vector< std::string >	Server::getServerNames( void ) const {
	return( _serverNames );
}

const std::vector< Location >		Server::getLocations( void ) const {
	return( _locations );
}

const std::map< int, std::string >	Server::getErrorPages( void ) const {
	return( _errorPages );
}

size_t				Server::getBodySize( void ) const {
	return( _bodySize );
}


//setter functions
void	Server::setIpPort( std::string tmp ){
	_ipPort = tmp;
}
void	Server::setServerName( const std::string name ){
	_serverNames.push_back( name );
}

void	Server::setLocation( Location location ){
	_locations.push_back( location );
}

void	Server::setErrorPage( int errorCode, std::string path ){
	_errorPages[ errorCode ] = path;
}

void	Server::setBodySize( size_t size ){
	_bodySize = size;
}
