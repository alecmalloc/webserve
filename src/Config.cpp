#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

//constructors
Config::Config( void ){
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
std::vector< Server >	Config::getServers( void ){
	return( _servers );
}

//setters
void	Config::setServer( Server& server ){
	_servers.push_back( server );
}

//memeber fucntions

static bool	checkIp( std::string tmp ){
	int	i = 0;
	int	check;
	size_t	pos;

	pos = tmp.find( '.' );
	while( pos != tmp.size() ){
		i++;
		if( i > 3 )
			return( false );
		std::string iptmp = tmp.substr( 0, pos );
		std::stringstream ss( iptmp );
		ss >> check;
		if ( check > 255 || check < 0 )
			return( false );
		tmp = tmp.substr( pos + 1, tmp.size() );
	}	
	return( true );
}

static bool	checkPort( std::string tmp ){
	std::stringstream	ss( tmp );
	int			check;

	ss >> check;
	if ( check < 0 || check > 65535 )
			return( false );
	return( true );
}

void	parseListen( Server& server, std::stringstream& ss ){
	std::string	tmp;
	std::string	ip( DEFAULT_IP );
	std::string	port( DEFAULT_PORT );

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	if( tmp.find( ':' ) != tmp.npos ){
		if ( !checkIp( tmp.substr( 0, tmp.find( ':' ) ) ) )
			throw( std::runtime_error( "Wrong IpAddress" + \
				tmp.substr( 0, tmp.find( ':' ) ) ) );
		if( !checkPort( tmp.substr( tmp.find( ':' ) + 1, tmp.size() ) ) )
			throw( std::runtime_error( "Wrong Port" + \
				tmp.substr( tmp.find( ':' ), tmp.size()  ) ) );
	}
	else if( tmp.find( '.' ) != tmp.npos ){
		if( !checkIp( tmp ) )
			throw( std::runtime_error( "Wrong IpAddress" + tmp ) );
		tmp = tmp.substr( 0, tmp.size() -1 ) + ":"  + port;
	}
	else{
		if( !checkPort( tmp ) )
			throw( std::runtime_error( "Wrong Port" + tmp ) );
		tmp = ip + ":" + tmp.substr(0, tmp.size() - 1 );
	}
	server.setIpPort( tmp );
}

void	parseServerName( Server& server, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp.at( tmp.size() - 1 ) != ';' ){
		server.setServerName( tmp );
		ss >> tmp;
	}
	server.setServerName( tmp.substr( 0, tmp.size() - 1 ) );
}

void	parseErrorPage( Server& server, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		int	error;
		std::stringstream	sstmp( tmp );
		sstmp >> error;
		//check if error code is valid
		ss >> tmp;
		//check if path is accesible 
		server.setErrorPage( error, tmp );
		ss >> tmp;
	}
}

void	parseBodySize( Server& server, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		std::stringstream	sstmp( tmp );
		int			size;

		sstmp >> size; 
		if( size < 0 || size > MAXBODYSIZE )
			throw( std::runtime_error( "Wrong Max Body Size" ) );
		server.setBodySize( size );
		ss >> tmp;
	}
}

void	parsePath( Location& location, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
//	if( tmp.at( tmp.size() - 1 ) != ';' )
//		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	location.setPath( tmp );
}


void	parseAllowedRedirects( Location& location, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		int	error;
		std::stringstream	sstmp( tmp );
		sstmp >> error;
		//check if error code is valid
		ss >> tmp;
		//check if path is accesible 
		location.setAllowedRedirects( error, tmp );
		ss >> tmp;
	}
}

void	parseAllowedMethods( Location& location, std::stringstream& ss ){
	std::string	tmp;
	std::string	allowedMethodes[] = ALLOWED_METHODES;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		for( size_t i = 0; \
		i == sizeof( allowedMethodes ) / sizeof( allowedMethodes[0] ); i++ ){
			if( i == sizeof( allowedMethodes ) / sizeof( allowedMethodes[0] ))
				throw( std::runtime_error( "Method not found " + tmp ) );
			else if( tmp == allowedMethodes[ i ] )
				location.setAllowedMethod( tmp );
		}	
		ss >> tmp;	
	}
}

void	parseRootDir( Location& location, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	//check for access
	location.setRootDir( tmp );
}

void	parseAutoIndex( Location& location, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		if( tmp == "true" )
			location.setAutoIndex( true );
		else if( tmp != "false" )
			location.setAutoIndex( false );
		else 
			throw( std::runtime_error( "Wrong Auto Index " + tmp ) );
		ss >> tmp;
	}

}

void	parseIndex( Location& location, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		//check for access
		location.setIndex( tmp );
		ss >> tmp;
	}
}

void	parseCgi( Location& location, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		std::string tmp2;
		ss >> tmp2;
		//check for endings
		location.setCgi( tmp, tmp2 );
		ss >> tmp;
	}
}

void	parseUploadDir( Location& location, std::stringstream& ss ){
	std::string	tmp;

	ss >> tmp;
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
	while( tmp != ";" ){
		//check for access
		location.setUploadDir( tmp );
		ss >> tmp;
	}

}


void	Config::parseLocationBlock( Server& server, std::stringstream& ss ){
	const std::string	optionsArray[] =  \
	{ "allowed_methods", "allowed_redirects", "root", "auto_index", \
		"index", "cgi", "upload_dir" };

	void ( *functionArray[] )( Location&, std::stringstream& ) = \
	{ parseAllowedMethods, parseAllowedRedirects, parseRootDir, \
		parseAutoIndex, parseIndex, parseCgi, parseUploadDir };

	Location	location;
	std::string	tmp;

	parsePath( location, ss );
	getline( _configFile, tmp );
	if ( tmp == "{" )
		getline( _configFile, tmp );
	while( tmp != "}" ){
		std::stringstream	ss( tmp );
		std::string		key;
		ss >> key;
		for( int i = 0; i < 7; i++ ){
			if( optionsArray[ i ] == key ){
				functionArray[ i ]( location, ss );
				break;
			}
			//else
			//	throw( std::runtime_error( "Not an valid configuration " 
			//		+ tmp ) );
		}
		getline( _configFile, tmp );
	}
	server.setLocation( location );	
}

void	Config::parseServerBlock( void ){
	const std::string	optionsArray[] =  \
	{ "listen", "server_name", "error_page", "client_max_body_size" };

	void ( *functionArray[] )( Server&, std::stringstream& ) = \
	{ parseListen, parseServerName, parseErrorPage, parseBodySize };

	std::string		tmp;
	Server			server;

	getline( _configFile, tmp );
	if ( tmp == "{" )
		getline( _configFile, tmp );
	while( tmp != "}"){
		std::stringstream	ss( tmp );
		std::string		key;
		ss >> key;
		for( int i = 0; i < 5; i++ ){
			if( optionsArray[ i ] == key ){
				functionArray[ i ]( server, ss );
				break;
			}
			else if ( key == "location" ){
				parseLocationBlock( server, ss );
				break;
			}
				// throw( std::runtime_error( "Not an valid configuration " 
				//	+ tmp ) );
		}
		getline( _configFile, tmp );
	}
	this->setServer( server );
}

void	Config::parse( std::string& filename ){
	std::string	tmp;

	_configFile.open( filename.c_str() );
	if( !_configFile.is_open() )
		throw( std::runtime_error( "Could not open Config File" ) );
	while( std::getline( _configFile, tmp ) ){
		if( tmp == "server" )
			this->parseServerBlock();	
	}
	if( _configFile.eof() && _servers.empty() )
		throw( std::runtime_error( "Did not find server block" ) );
}
