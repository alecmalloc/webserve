#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

//constructors
Config::Config( void ) :  _servers() {
	;
}

Config::Config( const Config& og ){
	_servers = og._servers;
}

Config::~Config( void ){
	;
}

//operator overloads
Config&		Config::operator =( const Config& og ){
	if( this != &og ){
		_servers = og._servers;
	}
	return( *this );
}

//getters
std::vector< ServerConf >	Config::getServerConfs( void ){
	return( _servers );
}

//setters
void	Config::setServerConf( ServerConf server ){
	_servers.push_back( server );
}

//memeber fucntions

void	Config::parse( std::string& filename ){
	std::string	tmp;

	_configFile.open( filename.c_str(), std::ios::in );
	if( !_configFile.is_open() )
		throw( std::runtime_error( "Could not open Config File" ) );
	std::getline( _configFile, tmp );
	while( !_configFile.eof() ){
		std::stringstream	ss( tmp );
		std::string		key;
		ss >> key;
		if( key == "server" ){
			ServerConf	server;
			this->parseServerConfBlock( server );	
			this->setServerConf( server );
		}
		getline( _configFile, tmp );
	}
	if( _configFile.eof() && _servers.empty() )
		throw( std::runtime_error( "Did not find server block" ) );
}
