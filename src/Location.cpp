#include "Location.hpp"

//constructors 
Location::Location( void ) {;}

Location::~Location( void ) {;}

Location::Location( const Location& og ){
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
}

//operator overloads
Location&	Location::operator =( const Location& og ){
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
	}
	return( *this );
}

std::ostream&	operator <<( std::ostream& os, const Location& loc ) {
    os << "Location Details:\n" << std::endl;
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
    	std::map< int, std::string >	tmp2 = loc.getAllowedRedirects();
    	for (std::map<int, std::string>::const_iterator it = tmp2.begin();
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
    	os << "Auto Index: " << "Enabled" << std::endl;
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
  const std::string                      Location::getPath( void ) const{
 	return( _path );
 }

 const std::vector< std::string >    Location::getAllowedMethods( void ) const{
 	return( _allowedMethods );
 }

 const std::map< int, std::string >     Location::getAllowedRedirects( void ) const{
 	return( _allowedRedirects );
 }

 const std::string                      Location::getRootDir( void ) const{
 	return( _rootDir );
 }

bool                             Location::getAutoIndex( void ) const{
	return( _autoIndex );	
 }

 const std::vector< std::string >       Location::getIndex( void ) const{
	 return( _index );
 }

 const std::vector< std::string >      Location::getCgiPath( void ) const{
	 return( _cgiPath );
 }

 const std::vector< std::string >      Location::getCgiExt( void ) const{
	 return( _cgiExt );
 }

 const std::string                       Location::getUploadDir( void ) const{
	 return( _uploadDir );
 }

//setter funcitons
void	Location:: setPath( std::string path ){
	_path =  path;
}

void	Location:: setAllowedMethod( std::string method ){
	_allowedMethods.push_back( method );
}

void	Location:: setAllowedRedirects( int redirect, std::string path ){
	_allowedRedirects[ redirect ] = path;
}

void	Location:: setRootDir( std::string rootDir ){
	_rootDir = rootDir;
}

void	Location:: setAutoIndex( bool status ){
	_autoIndex = status;
}

void	Location:: setIndex( std::string index ){
	_index.push_back( index );
}

void	Location:: setCgiPath( std::string key ){
	_cgiPath.push_back(  key );
}

void	Location:: setCgiExt( std::string key ){
	_cgiExt.push_back( key );
}

void	Location:: setUploadDir( std::string uploadDir ){
	_uploadDir = uploadDir;
}
