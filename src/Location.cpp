#include "../inc/Location.hpp"

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
	_cgi = og._cgi;
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
		_cgi = og._cgi;
		_uploadDir = og._uploadDir;
	}
	return( *this );
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

 const std::map< std::string, std::string >      Location::getCgi( void ) const{
	 return( _cgi );
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

void	Location:: setCgi( std::string key, std::string value ){
	_cgi[ key ] = value;
}

void	Location:: setUploadDir( std::string uploadDir ){
	_uploadDir = uploadDir;
}

