#include "Location.hpp"

//constructors 
LocationConf::LocationConf( void ) : _autoIndex( 0 ), _bodySizeInitilized( false ), _bodySize( 0 ) {;}

LocationConf::~LocationConf( void ) {;}

LocationConf::LocationConf( const LocationConf& og ){
	_path = og._path;
	_allowedMethods = og._allowedMethods;
	_allowedRedirects = og._allowedRedirects;
	_rootDir = og._rootDir;
	_autoIndex = og._autoIndex;
	_rootDir = og._rootDir;
	_autoIndex = og._autoIndex;
	_index = og._index;
	_cgiPath = og._cgiPath;
	_cgiExt = og._cgiExt;
	_uploadDir = og._uploadDir;
	_bodySize = og._bodySize;
	_bodySizeInitilized = og._bodySizeInitilized;
}

//operator overloads
LocationConf&	LocationConf::operator =( const LocationConf& og ){
	if ( this != & og ){
		_path = og._path;
		_allowedMethods = og._allowedMethods;
		_allowedRedirects = og._allowedRedirects;
		_rootDir = og._rootDir;
		_autoIndex = og._autoIndex;
		_rootDir = og._rootDir;
		_autoIndex = og._autoIndex;
		_index = og._index;
		_cgiPath = og._cgiPath;
		_cgiExt = og._cgiExt;
		_uploadDir = og._uploadDir;
		_bodySize = og._bodySize;
		_bodySizeInitilized = og._bodySizeInitilized;
	}
	return( *this );
}

std::ostream&	operator <<( std::ostream& os, const LocationConf& loc ) {
	std::vector< std::string > tmp;

    // Print path
    if ( !loc.getPath().empty() ){
    	os << "Path: " << loc.getPath() << std::endl;
    }

    // Print allowed methods
    if ( !loc.getAllowedMethods().empty() ){
    	os << "Allowed Methods: ";
    	tmp = loc.getAllowedMethods();
    	for (std::vector<std::string>::const_iterator it = tmp.begin();
    	     it != tmp.end(); ++it) {
    	    os << *it << " ";
    	}
    	os << std::endl;
    }

    // Print allowed redirects
    if ( !loc.getAllowedRedirects().empty() ){
    	os << "Allowed Redirects:" << std::endl;
    	std::map< std::string, std::string >	tmp2 = loc.getAllowedRedirects();
    	for (std::map<std::string, std::string>::const_iterator it = tmp2.begin();
    	     it != tmp2.end(); ++it) {
    	    os << "  Code " << it->first << ": " << it->second << std::endl;
    	}
    }

    // Print root directory
    if ( !loc.getRootDir().empty() ){
    	os << "Root Directory: " << loc.getRootDir() << std::endl;
    }

    // Print auto index setting
    if ( loc.getAutoIndex() == true ){
    	os << "Auto Index: " << "On" << std::endl;
    }

    // Print index files
    if ( !loc.getIndex().empty() ){
    	os << "Index Files: ";
    	tmp = loc.getIndex();
    	for (std::vector<std::string>::const_iterator it = tmp.begin();
    	     it != tmp.end(); ++it) {
    	    os << *it << " ";
    	}
    	os << std::endl;
    }

    // Print CGI paths
    if ( !loc.getCgiPath().empty() ){
   	 os << "CGI Paths: ";
   	 tmp = loc.getCgiPath();
   	 for (std::vector<std::string>::const_iterator it = tmp.begin();
   	      it != tmp.end(); ++it) {
   	     os << *it << " ";
   	 }
   	 os << std::endl;
    }

    // Print CGI extensions
    if ( !loc.getCgiExt().empty() ) {
    	os << "CGI Extensions: ";
    	tmp = loc.getCgiExt();
    	for (std::vector<std::string>::const_iterator it = tmp.begin();
    	     it != tmp.end(); ++it) {
    	    os << *it << " ";
    	}
    	os << std::endl;
    }

    // Print upload directory
    if ( !loc.getUploadDir().empty() ){
    	os << "Upload Directory: " << loc.getUploadDir() << std::endl;
    }

    return os;
}

//getter functions
  const std::string                      LocationConf::getPath( void ) const{
 	return( _path );
 }

 const std::vector< std::string >    LocationConf::getAllowedMethods( void ) const{
 	return( _allowedMethods );
 }

 const std::map< std::string, std::string >     LocationConf::getAllowedRedirects( void ) const{
 	return( _allowedRedirects );
 }

 const std::string                      LocationConf::getRootDir( void ) const{
 	return( _rootDir );
 }

int                             LocationConf::getAutoIndex( void ) const{
	return( _autoIndex );	
 }

 const std::vector< std::string >       LocationConf::getIndex( void ) const{
	 return( _index );
 }

 const std::vector< std::string >      LocationConf::getCgiPath( void ) const{
	 return( _cgiPath );
 }

 const std::vector< std::string >      LocationConf::getCgiExt( void ) const{
	 return( _cgiExt );
 }

 const std::string                       LocationConf::getUploadDir( void ) const{
	 return( _uploadDir );
 }

size_t	LocationConf::getBodySize( void ) const{
	return( _bodySize );
}

bool	LocationConf::getBodySizeInitilized( void ) const{
	return( _bodySizeInitilized );
}


//setter funcitons
void	LocationConf:: setPath( std::string path ){
	_path =  path;
}

void	LocationConf:: setAllowedMethod( std::string method ){
	_allowedMethods.push_back( method );
}

void	LocationConf:: setAllowedRedirects( std::string redirect, std::string path ){
	_allowedRedirects[ redirect ] = path;
}

void	LocationConf:: setRootDir( std::string rootDir ){
	_rootDir = rootDir;
}

void	LocationConf:: setAutoIndex( int status ){
	_autoIndex = status;
}

void	LocationConf:: setIndex( std::string index ){
	_index.push_back( index );
}

void	LocationConf:: setCgiPath( std::string key ){
	_cgiPath.push_back(  key );
}

void	LocationConf:: setCgiExt( std::string key ){
	_cgiExt.push_back( key );
}

void	LocationConf:: setUploadDir( std::string uploadDir ){
	_uploadDir = uploadDir;
}

void	LocationConf::setBodySize( size_t val ){
	_bodySize = val;
	_bodySizeInitilized = true;
}
