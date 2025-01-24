#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

static std::string	cutEnding( std::string tmp ){
	//if string ends on ; cut it
	if( tmp.at( tmp.size() - 1 ) == ';' )
		tmp = tmp.substr( 0, tmp.size() - 1 );

	return( tmp );
}

static std::string	makePath( std::string tmp, int root ){
	if( tmp.at( 0 ) != '/' && !root )
		tmp = "/" + tmp;
	if( tmp.at( tmp.size() - 1 ) == '/' )
		tmp = tmp.substr( 0, tmp.size() - 1 );
	return( tmp );
}

static bool	validRedirect( std::string tmp ) {
	//check if redirect is alloewd
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

void	parsePath( LocationConf& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		if( tmp.find( '{' ) == tmp.npos ){
			location.setPath( makePath( cutEnding( tmp ), 0 ) );
		}
	}
}

void	parseAllowedRedirects( LocationConf& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		std::string	error;
		std::stringstream	sstmp( tmp );
		sstmp >> error;
		if( !validRedirect( error ) )
			throw( std::runtime_error( "Redirect Code " + error \
				+ " not allowed" ) );
		ss >> tmp;
		location.setAllowedRedirects( error, makePath( cutEnding( tmp ), 0 ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseAllowedMethods( LocationConf& location, std::stringstream& ss ){
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

	while( ss >> tmp )
		temp.setRootDir( makePath( cutEnding( tmp ), 1 ) );
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseAutoIndex( LocationConf& location, std::stringstream& ss ){
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

	while( ss >> tmp )
		temp.setIndex( makePath( cutEnding( tmp ), 0 ) );
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseCgiPath( LocationConf& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		location.setCgiPath( cutEnding( tmp ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}


void	parseCgiExt( LocationConf& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp ){
		//TODO:check for endings
		location.setCgiExt( cutEnding( tmp ) );
	}
	if( tmp.at( tmp.size() - 1 ) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	parseUploadDir( LocationConf& location, std::stringstream& ss ){
	std::string	tmp;

	while( ss >> tmp )
		location.setUploadDir( makePath( cutEnding( tmp ), 0 ) );
	if( tmp.at( tmp.size() - 1) != ';' )
		throw( std::runtime_error( "Line not ended on ; " + tmp ) );
}

void	Config::parseLocationConfBlock( ServerConf& server, std::stringstream& ss ){
	const std::string	optionsArray[] =  \
	{ METHODE, REDIRECT, ROOT, AINDEX, INDEX, CGIPATH, CGIEXT, UPDIR };

	void ( *functionArray[] )( LocationConf&, std::stringstream& ) = \
	{ parseAllowedMethods, parseAllowedRedirects, parseRootDir, \
		parseAutoIndex, parseIndex, parseCgiPath, parseCgiExt, parseUploadDir };

	LocationConf	location;
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
	server.setLocationConf( location );	
}
