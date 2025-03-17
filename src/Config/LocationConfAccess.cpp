#include "webserv.hpp"

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

void	LocationConf::checkAccess( std::string rootDir ){
	//check if root dir is set && accessible
	if( !_rootDir.empty() && !accessibleDir( _rootDir ) )
		throw( std::runtime_error( "Location rootDir " + _rootDir \
			+ " not accessible" ) );
	else if( !_rootDir.empty() && accessibleDir( _rootDir ) )
		rootDir = _rootDir;

	//check access to Path
	if( !_path.empty() && !accessibleDir( rootDir + _path ) )
		throw( std::runtime_error( "Location Path " + rootDir + _path + \
			" not accessible" ) );

	//check access to index files
	for( std::vector< std::string >::iterator it = _index.begin(); \
			it != _index.end(); it++ ){
		if( !_index.empty() && !accessibleFile( rootDir + _path + *it ) )
			throw( std::runtime_error( "Index file " + rootDir + _path + \
					*it + " not accessible" ) );
	}

	/*check upload dir ---> think we dont need that cause i make the dir if needed like Nginx would 
	if( !_uploadDir.empty() && !accessibleDir( rootDir + _path + _uploadDir ) )
		throw( std::runtime_error( "UploadDir " + rootDir + _path + _uploadDir + \
				" not accesible" ) );
	*/
				
	//check allowed redirect
	for( std::map< std::string, std::string >::iterator it = \
			_allowedRedirects.begin(); it != _allowedRedirects.end(); it++ ){
		if( !accessibleFile( rootDir + _path + it->second ) )
			throw( std::runtime_error( "Allowed Redirect " + rootDir + \
					_path + it->second + " not accessible" ) );
	}
}
