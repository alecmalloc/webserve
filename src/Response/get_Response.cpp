#include "webserv.hpp"

//serve Files
void	Response::serveFileIfExists( const std::string& fullPath ){

	std::ifstream file( fullPath.c_str(), std::ios::in | std::ios::binary );

	if( !file )
	       throw( 500 );	

	std::ostringstream	contents;

	contents << file.rdbuf();
	file.close();

	setBody( contents.str() );

	setResponseCode(200);
}

//server file from _serverConf
void	Response::serveRootIndexfile( void ){

	std::string			fullPath = _pathInfo.getFullPath();
	std::vector<std::string>	indexFiles = _serverConf->getIndex();

	if( !indexFiles.empty() ){
		for (std::vector<std::string>::iterator it = indexFiles.begin(); \
		     it != indexFiles.end(); ++it){

			//store each index path
			const std::string& indexFile = *it;

			// Construct the path to the index file
			std::string rootIndexPath = _serverConf->getRootDir() + "/" + indexFile;

			//check for access and serve
			if( access( rootIndexPath, R_OK ) == 0 )
				serveFileIfExists(rootIndexPath, _request);
			
		}
	}

	//if no index check for autodirectory listing
	else if( _serverConf->getAutoIndex() == true ){
		setBody( generateDirectoryListing( fullPath ) );
		_request.setResponseCode(200);
	}

	//no root index file
	else
		throw( 404 );
}

//serve file from locationConf
void Response::serveLocationIndex( void ){

	std::string			fullPath = _pathInfo.getFullPath();
	std::vector<std::string>	indexFiles = _locationConf->getIndex();

	if( !indexFiles.empty() ){
		for (std::vector<std::string>::iterator it = indexFiles.begin(); \
		     it != indexFiles.end(); ++it){

			//store each index path
			const std::string& indexFile = *it;

			//TODO::check if location index path is already with root dir
			// Construct the path to the index file
			std::string rootIndexPath = indexFile;

			//check for access and serve
			if( access( rootIndexPath, R_OK ) == 0 )
				serveFileIfExists(rootIndexPath, _request);
			
		}
	}

	//if no index check for autodirectory listing
	else if( _locationConf->getAutoIndex() == true ){
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

void	Response::HandleGetRequest( void ){

	std::string uri = _request.getUri();
	std::string fullPath = _pathInfo.getFullPath();

	// custom cookies override - Alec
	// check if the uri points at the /customCookiesEndpoint/CookiesPage end point
	if( isCookie( uri ) )
		handleCookiesPage(_request);

	//if request is a file -> serve
	else if( _pathInfo.isFile() )
		serveFileIfExists(fullPath, _request);
	

	//if request is root dir -> serve
	else if( fullPath == _serverConf->getRootDir() )
		serveRootIndexfile(_request, fullPath);
	
	//TODO::check with wrong search if 404 is returned
	//serve locationblock
	else
		serveLocationIndex();
}

