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
void	Config::setServer( Server server ){
	_servers.push_back( server );
}

//memeber fucntions

static std::string	cutEnding( std::string tmp ){
	if( tmp.at( tmp.size() - 1 ) == ';' )
		tmp = tmp.substr( 0, tmp.size() - 1 );
	return( tmp );
}

static bool	accessibleFile( std::string tmp ){
	if( access( cutEnding( tmp ).c_str(), R_OK ) == 0 )
		return( true );
	else
		return( false );
}

static bool	accessibleDir( std::string tmp ){
	struct stat	info;

	if( stat( cutEnding( tmp ).c_str(), &info ) != 0 )	return false;
	else if( info.st_mode & S_IFDIR )	return true;
	else 					return false;
}

static bool	validRedirect( std::string tmp ) {
	std::string	allowedRedirects[] = ALLOWED_REDIRECTS;
	for( int i = 0; \
		i != sizeof( allowedRedirects ) / sizeof( allowedRedirects[0] ) + 1; \
		i++ ){
		if ( i == sizeof( allowedRedirects ) / sizeof( allowedRedirects[0] ) )
			return( false );
		else if ( allowedRedirects[i] == tmp )
			return( true );
	}
	return( false );
}

static bool	checkIp( std::string tmp ){
	int	i = 0;
	int	check;
	size_t	pos;

	pos = tmp.find( '.' );
	while( pos != tmp.npos ){
		std::string iptmp = tmp.substr( i, pos );
		std::stringstream ss( iptmp );
		ss >> check;
		if ( check > 255 || check < 0 )
			return( false );
		if( tmp.at( pos + 1 ) )
			tmp = tmp.substr( pos + 1, tmp.size() );
		i = pos;
		pos = tmp.find( '.' );
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
	server.setIpPort( cutEnding( tmp ) );
}

void	parseServerName( Server& server, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		server.setServerName( cutEnding( tmp ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseErrorPage( Server& server, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		int	error;
		std::stringstream	sstmp( tmp );
		sstmp >> error;
		//check if error code is valid
		ss >> tmp;
		if( accessibleFile( tmp ) )
			server.setErrorPage( error, cutEnding( tmp ) );
		else
			throw( std::runtime_error( "Error Page " + tmp \
						+ " not accessable" ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseBodySize( Server& server, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		std::stringstream	sstmp( tmp );
		int			size;

		sstmp >> size; 
		if( size < 0 || size > MAXBODYSIZE )
			throw( std::runtime_error( "Wrong Max Body Size" ) );
		server.setBodySize( size );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parsePath( Location& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		if( tmp.find( '{' ) == tmp.npos )
			location.setPath( cutEnding( tmp ) );
	}
}


void	parseAllowedRedirects( Location& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		std::string	error;
		std::stringstream	sstmp( tmp );
		sstmp >> error;
		if( !validRedirect( error ) )
			throw( std::runtime_error( "Redirect Code " + error \
				+ " not allowed" ) );
		ss >> tmp;
		if(  accessibleFile( tmp ) )
			location.setAllowedRedirects( error, cutEnding( tmp ) );
		else
			throw( std::runtime_error( "Redirect " + tmp \
						+ " not accessable" ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseAllowedMethods( Location& location, std::stringstream& ss ){
	std::string	tmp;
	std::string	allowedMethodes[] = ALLOWED_METHODES;

	while( ss >> tmp ){
		for( size_t i = 0; \
		i != ( sizeof( allowedMethodes ) / sizeof( allowedMethodes[0] ) + 1 ); \
		i++ ){
			if( i == sizeof( allowedMethodes ) / sizeof( allowedMethodes[0] ))
				throw( std::runtime_error( "Method not found " + tmp ) );
			else if( cutEnding( tmp ) == allowedMethodes[ i ] ){
				location.setAllowedMethod( cutEnding( tmp ) );
				break;
			}
		}	
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

template< typename T >
void	parseRootDir( T& temp, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		if( accessibleDir( tmp ) )
			temp.setRootDir( cutEnding( tmp ) );
		else
			throw( std::runtime_error( "Root Dir " + tmp \
						+ " not accessable" ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseAutoIndex( Location& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		if( cutEnding( tmp ) == "on" )
			location.setAutoIndex( true );
		else if( cutEnding( tmp ) == "off" )
			location.setAutoIndex( false );
		else 
			throw( std::runtime_error( "Wrong Auto Index " + tmp ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

template< typename T>
void	parseIndex( T& temp, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		if( accessibleFile( tmp ) )
			temp.setIndex( cutEnding( tmp ) );
		else
			throw( std::runtime_error( "Index " + tmp \
						+ " not accessable" ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseCgiPath( Location& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		location.setCgiPath( cutEnding( tmp ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}


void	parseCgiExt( Location& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		//check for endings
		location.setCgiExt( cutEnding( tmp ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseUploadDir( Location& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		if( accessibleDir( tmp ) )
			location.setUploadDir( cutEnding( tmp ) );
		else
			throw( std::runtime_error( "Upload Dir " + tmp \
						+ " not accessable" ) );
	}
	if( tmp.at( tmp.size() - 1) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}


void	Config::parseLocationBlock( Server& server, std::stringstream& ss ){
	const std::string	optionsArray[] =  \
	{ METHODE, REDIRECT, ROOT, AINDEX, INDEX, CGIPATH, CGIEXT, UPDIR };

	void ( *functionArray[] )( Location&, std::stringstream& ) = \
	{ parseAllowedMethods, parseAllowedRedirects, parseRootDir, \
		parseAutoIndex, parseIndex, parseCgiPath, parseCgiExt, parseUploadDir };

	Location	location;
	std::string	tmp;

	parsePath( location, ss );
	getline( _configFile, tmp );
	if ( tmp == "{" )
		getline( _configFile, tmp );
	while( tmp.find( '}' ) == tmp.npos ){
		std::stringstream	ss( tmp );
		std::string		key;
		ss >> key;
		for( int i = 0; i < 8; i++ ){
			if( optionsArray[ i ] == key ){
				functionArray[ i ]( location, ss );
				break;
			}
			else if ( i == 7 && !key.empty() )
				throw( std::runtime_error( "Not an valid configuration " 
							+ tmp ) );
		}
		getline( _configFile, tmp );
	}
	server.setLocation( location );	
}

void	Config::parseServerBlock( Server& server ){
	const std::string	optionsArray[] =  \
	{ LISTEN, SERVER, ERROR, CLIENT, ROOT, INDEX };

	void ( *functionArray[] )( Server&, std::stringstream& ) = \
	{ parseListen, parseServerName, parseErrorPage, parseBodySize, \
		parseRootDir, parseIndex };

	std::string		tmp;

	getline( _configFile, tmp );
	if ( tmp == "{" )
		getline( _configFile, tmp );
	while( tmp.find( '}' ) == tmp.npos ){
		std::stringstream	ss( tmp );
		std::string		key;
		ss >> key;
		for( int i = 0; i < 7; i++ ){
			if( !key.empty() && key.at( 0 ) == '#' ){
				break;
			}
			if( optionsArray[ i ] == key ){
				functionArray[ i ]( server, ss );
				break;
			}
			else if ( key == "location" ){
				parseLocationBlock( server, ss );
				break;
			}
			else if ( i == 6 && !key.empty() ){
				throw( std::runtime_error( "Not an valid configuration "\
							+ tmp ) );

			}
		}
		getline( _configFile, tmp );
	}
}

void	Config::parse( std::string& filename ){
	std::string	tmp;

	_configFile.open( filename.c_str(), std::ios::in );
	if( !_configFile.is_open() )
		throw( std::runtime_error( "Could not open Config File" ) );
	std::getline( _configFile, tmp );
	while( tmp.find( '}' ) == tmp.npos && !_configFile.eof() ){
		std::stringstream	ss( tmp );
		std::string		key;
		ss >> key;
		if( key == "server" ){
			Server	server;
			this->parseServerBlock( server );	
			this->setServer( server );
		}
		getline( _configFile, tmp );
	}
	if( _configFile.eof() && _servers.empty() )
		throw( std::runtime_error( "Did not find server block" ) );
}
