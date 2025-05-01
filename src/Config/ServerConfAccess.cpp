#include "webserv.hpp"
#include <string>

static std::string	cutEnding( std::string tmp ){
	//if string ends on ; cut it
	if( tmp.at( tmp.size() - 1 ) == ';' )
		tmp = tmp.substr( 0, tmp.size() - 1 );

	return( tmp );
}

static bool	accessibleFile( std::string tmp ){
	//test function if path tmp is accessible
	if( access( cutEnding( tmp ).c_str(), R_OK ) == 0 )
		return( true );
	else
		return( false );
}

static bool	accessibleDir( std::string tmp ){
	//test function if path tmp is accasible
	struct stat	info;

	if( stat( cutEnding( tmp ).c_str(), &info ) != 0 )	return false;
	else if( info.st_mode & S_IFDIR )			return true;
	else 							return false;
}

void	ServerConf::checkAccess( void ){
	//check root dir 
	if( !_rootDir.empty() &&  !accessibleDir( _rootDir ) )
		throw( std::runtime_error( \
			"RootDir: " + _rootDir + " not accsessible" ) );

	//check every errorpage path
	for( std::map< int, std::string >::iterator it = _errorPages.begin(); \
			it != _errorPages.end(); it++ ){
		if( !accessibleFile( _rootDir + it->second ) )
			throw( std::runtime_error( "ErrorPage: " \
			       	+ _rootDir + it->second + " not accsessible" ) );
	}

	//check every index path
	for( std::vector< std::string >::iterator it = _index.begin(); \
			it != _index.end(); it++ ){
		if( !accessibleFile( _rootDir + *it ) )
			throw( std::runtime_error( \
				"Indexfile: " + _rootDir + *it + " not accessible" ) );
	}

	//check if hostnames is set
	if( _serverNames.empty() )
		throw( std::runtime_error( "Every Serverblock needs an Hostname" ) );
}
