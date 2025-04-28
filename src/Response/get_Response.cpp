#include "webserv.hpp"

//serve Files
static std::string	serveFileIfExists( const std::string& fullPath ){

	std::ifstream file( fullPath.c_str(), std::ios::in | std::ios::binary );

	if( !file )
	       throw( 500 );	

	std::ostringstream	contents;

	contents << file.rdbuf();
	file.close();

	return( contents.str() );
}

//server file from _serverConf
void	Response::serveRootIndex( void ){

	std::string			fullPath = _pathInfo.getFullPath();
	std::vector<std::string>	indexFiles = _serverConf.getIndex();

	if( !indexFiles.empty() ){
		for (std::vector<std::string>::iterator it = indexFiles.begin(); \
		     it != indexFiles.end(); ++it){

			//store each index path
			const std::string& indexFile = *it;

			// Construct the path to the index file
			std::string rootIndexPath = _serverConf.getRootDir() + "/" + indexFile;

			//check for access and serve
			if( access( rootIndexPath.c_str(), R_OK ) == 0 ){
				setBody( serveFileIfExists( rootIndexPath ) );
				setStatus( 200 );
			}
		}
	}

	//if no index check for autodirectory listing
	else if( _serverConf.getAutoIndex() == true ){
		setBody( generateDirectoryListing( fullPath ) );
		_request.setResponseCode( 200 );
	}

	//no root index file
	else
		throw( 404 );
}

bool	checkAutoIndex( LocationConf& loc, ServerConf& ser ){
	if( loc.getAutoIndex() == -1 )
		throw( 403 );
	return( loc.getAutoIndex() ? loc.getAutoIndex() : ser.getAutoIndex() );
}

//serve file from locationConf
void	Response::serveLocationIndex( void ){

	std::string			fullPath = _pathInfo.getFullPath();
	std::vector<std::string>	indexFiles = _locationConf.getIndex();

	if( !indexFiles.empty() ){
		for (std::vector<std::string>::iterator it = indexFiles.begin(); \
		     it != indexFiles.end(); ++it){

			//store each index path
			const std::string& indexFile = *it;

			// Construct the path to the index file
			std::string rootIndexPath = fullPath + indexFile;

			//check for access and serve
			if( access( rootIndexPath.c_str(), R_OK ) == 0 ){
				setBody( serveFileIfExists( rootIndexPath ) );
				setStatus( 200 );
				break;
			}
		}
	}

	//if no index check for autodirectory listing
	else if( checkAutoIndex( _locationConf, _serverConf ) ){
		setBody( generateDirectoryListing( fullPath ) );
		_request.setResponseCode(200);
	}

	//no root index file
	else
		throw( 404 );
}

static bool	isCookie( std::string uri ){
	if (uri == "/customCookiesEndpoint/CookiesPage" || \
	    uri == "/customCookiesEndpoint/CookiesPage/deactivate" ||\
	    uri == "/customCookiesEndpoint/CookiesPage/activate")
		return( true );
	return( false );
}

void	Response::handleGetRequest( void ){

	std::string uri = _request.getUri();
	std::string fullPath = _pathInfo.getFullPath();

	// custom cookies override - Alec
	// check if the uri points at the /customCookiesEndpoint/CookiesPage end point
	if( isCookie( uri ) ) 
		handleCookiesPage(_request);
	//if request is a file -> serve
	else if( _pathInfo.isFile() ){
		setBody( serveFileIfExists( fullPath ) );
		setStatus( 200 );
	}
	//if request is root dir -> serve
	else if( fullPath == _serverConf.getRootDir() + "/" )
		serveRootIndex();
	//serve locationblock
	else if( _isLocation )
		serveLocationIndex();
	//404 not found	else
	else
		throw( 404 );
}

