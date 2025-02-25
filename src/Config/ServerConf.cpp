#include "webserv.hpp"
#include <ostream>
#include <string>
#include <vector>

//constructors

ServerConf::ServerConf( void ) : _bodySize( 0 ) {;}

ServerConf::ServerConf( const ServerConf& og ){
	_ipPort = og._ipPort;
	_serverNames = og._serverNames;
	_locations = og._locations;
	_errorPages = og._errorPages;
	_bodySize = og._bodySize;
	_rootDir = og._rootDir;
	_index = og._index;
}

ServerConf::~ServerConf( void ) {;}

//operator overloads

ServerConf&		ServerConf::operator =( const ServerConf& og ) {
	if ( this != &og ){
		_ipPort = og._ipPort;
		_serverNames = og._serverNames;
		_locations = og._locations;
		_errorPages = og._errorPages;
		_bodySize = og._bodySize;
		_rootDir = og._rootDir;
		_index = og._index;
	}
	return( *this );
}

std::ostream&	operator <<( std::ostream& os, const ServerConf& server) {

	os << "ServerConf Details:\n" << std::endl;
	std::vector< std::string >	tmp;

	// Print IP and Port
	if ( !server.getIpPort().empty() ){
		std::map< std::string, std::set< int > > tmp2 = server.getIpPort();
		for( std::map< std::string, std::set< int > >::const_iterator it = \
				tmp2.begin(); it != tmp2.end(); it++ ){
			os << "ip: " << it->first << " ports: ";
			for( std::set< int >::iterator it2 = it->second.begin(); \
					it2 != it->second.end(); it2++ ){
				os << *it2 << " ";
			}
			os << "\n";
		}
	}

	// Print ServerConf Names
	if ( !server.getServerConfNames().empty() ){
		os << "ServerConf Names: ";
		std::vector< std::string > tmp = server.getServerConfNames();
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

	//print RootDir
	if ( !server.getRootDir().empty() )
		os << "RootDir: " << server.getRootDir() << "\n";

	// Print index files
	if ( !server.getIndex().empty() ){
		os << "Index Files: ";
		tmp = server.getIndex();
		for (std::vector<std::string>::const_iterator it = tmp.begin();
				it != tmp.end(); ++it) {
			os << *it << " ";
		}
		os << std::endl;
	}

	// Print LocationConfs
	if ( !server.getLocationConfs().empty() ){
		os << "LocationConfs:\n";
		int	i = 0;
		std::vector< LocationConf >	tmp3 = server.getLocationConfs();
		for (std::vector<LocationConf>::const_iterator it = tmp3.begin(); 
				it != tmp3.end(); ++it) {
			os << i << ":\n" << *it << "\n";
			i++;
		}
	}
	return( os );
}


//getter functions

bool ServerConf::getChunkedTransfer( void ) const {
    return _chunkedTransfer;
}

size_t ServerConf::getChunkSize( void ) const {
    return _chunkSize;
}


const std::map< std::string, std::set< int > >	ServerConf::getIpPort( void ) const {
	return( _ipPort );
}

const std::vector< std::string >	ServerConf::getServerConfNames( void ) const {
	return( _serverNames );
}

const std::vector< LocationConf >		ServerConf::getLocationConfs( void ) const {
	//std::cout << "Debug: Getting location confs" << std::endl;
    //std::cout << "Debug: Server name: " << getRootDir() << std::endl;
	return( _locations );
}

const std::map< int, std::string >	ServerConf::getErrorPages( void ) const {
	return( _errorPages );
}

size_t				ServerConf::getBodySize( void ) const {
	return( _bodySize );
}

const std::string	ServerConf::getRootDir( void ) const {
	return( _rootDir );
}

const std::vector< std::string >	ServerConf::getIndex( void ) const{
	return( _index );
}

//setter functions

void ServerConf::setChunkedTransfer( bool chunked ) {
    _chunkedTransfer = chunked;
}

void ServerConf::setChunkSize( size_t size ) {
    _chunkSize = size;
}

void	ServerConf::setIpPort( std::string ip, int port ){
	_ipPort[ ip ].insert( port );
}
void	ServerConf::setServerConfName( const std::string name ){
	_serverNames.push_back( name );
}

void	ServerConf::setLocationConf( LocationConf location ){
	_locations.push_back( location );
}

void	ServerConf::setErrorPage( int errorCode, std::string path ){
	_errorPages[ errorCode ] = path;
}

void	ServerConf::setBodySize( size_t size ){
	_bodySize = size;
}

void	ServerConf::setRootDir( std::string tmp ){
	_rootDir = tmp;
}

void	ServerConf::setIndex( std::string tmp ){
	_index.push_back( tmp );
}
