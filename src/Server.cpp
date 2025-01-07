#include "Server.hpp"
#include <string>

//constructors

Server::Server( void ) : _bodySize( 0 ) {;}

Server::Server( const Server& og ){
	_serverNames = og._serverNames;
	_locations = og._locations;
	_errorPages = og._errorPages;
	_bodySize = og._bodySize;
}

Server::~Server( void ) {;}

//operator overloads

Server&		Server::operator =( const Server& og ) {
	if ( this != &og ){
		_serverNames = og._serverNames;
		_locations = og._locations;
		_errorPages = og._errorPages;
		_bodySize = og._bodySize;
	}
	return( *this );
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
void	Server::setIpPort( std::string& tmp ){
	_ipPort = tmp;
}
void	Server::setServerName( const std::string& name ){
	_serverNames.push_back( name );
}

void	Server::setLocation( Location& location ){
	_locations.push_back( location );
}

void	Server::setErrorPage( int errorCode, std::string& path ){
	_errorPages[ errorCode ] = path;
}

void	Server::setBodySize( size_t size ){
	_bodySize = size;
}
