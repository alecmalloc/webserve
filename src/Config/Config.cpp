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
void		Config::setServerConf( ServerConf server ){
	_servers.push_back( server );
}

//memeber fucntions

static void	checkHostnames( ServerConf i, ServerConf j ){
	
	std::vector<std::string>		hay = i.getServerConfNames();
	std::vector<std::string>		needles = j.getServerConfNames();
	std::vector<std::string>::iterator	it;
	
	for( it = needles.begin(); it != needles.end(); it++ ){

		std::string	needle = *it;
		if( std::find( hay.begin(), hay.end(), needle ) != hay.end() )
			throw( std::runtime_error( "DiffHostnames for virtual Hosting requiered" ) );
	}
}

static int	checkSet( std::map< std::string, std::set<int> >::iterator tmp, \
			std::map< std::string, std::set<int> >::const_iterator m ){
	
	std::set<int>::iterator	s;

	for( s = m->second.begin(); s != m->second.end(); s++ ){
		if( tmp->second.find( *s ) != tmp->second.end() )
			return( 1 );
	}
	
	return( 0 );
}

static int	checkMap( std::map< std::string, std::set<int> >::const_iterator m, \
			std::map< std::string, std::set<int> > map ){
	
	std::map< std::string, std::set< int > >::iterator tmp;

	tmp = map.find( m->first );

	if( tmp != map.end() )
		return( checkSet( tmp, m ) );
	return( 0 );
}

static void	checkPortsandHostnames( std::vector< ServerConf > s ){

	std::vector< ServerConf >::iterator i;
	for( i = s.begin(); i != s.end(); i++ ){

		std::map< std::string, std::set<int> >::const_iterator	m;
		const std::map< std::string, std::set<int> >&			map = i->getIpPort();				
		for( m = map.begin(); m != map.end(); m++ ){
			
			std::vector< ServerConf >::iterator j;
			for( j = i + 1; j != s.end(); j++ ){

				if( checkMap( m, j->getIpPort() ) )
					checkHostnames( (*i), (*j) );
			}	
		}
	}
}

void		Config::parse( std::string& filename ){

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
	checkPortsandHostnames( _servers );
}
