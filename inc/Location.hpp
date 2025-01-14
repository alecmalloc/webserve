#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <map>
#include <string>
#include <ostream>

class Location {
	public:
		//constructors
		Location( void );
		Location( const Location& og );
		~Location( void );

		//operator overloads
		Location&	operator =( const Location& og );

		//getter functions
		const std::string			getPath( void ) const;
		const std::vector< std::string >	getAllowedMethods( void ) const;
		const std::map< std::string, std::string >	getAllowedRedirects( void ) const;
		const std::string			getRootDir( void ) const;
		bool				getAutoIndex( void ) const;
		const std::vector< std::string >	getIndex( void ) const;
		const std::vector< std::string >	getCgiPath( void ) const;
		const std::vector< std::string >	getCgiExt( void ) const;
		const std::string			getUploadDir( void ) const;

		//setter functions
		void	setPath( std::string );
		void	setAllowedMethod( std::string );
		void	setAllowedRedirects( std::string, std::string );
		void	setRootDir( std::string );
		void	setAutoIndex( bool );
		void	setIndex( std::string );
		void	setCgiPath( std::string );
		void	setCgiExt( std::string );
		void	setUploadDir( std::string );

		//member functions


	private: 
		//location Path
		std::string			_path;

		//store http methiods
		std::vector< std::string >	_allowedMethods;

		//store http redirects
		std::map< std::string, std::string >	_allowedRedirects;

		//root directory
		std::string			_rootDir;

		//directory listing
		bool				_autoIndex;

		//index
		std::vector< std::string >	_index;

		//cgi
		std::vector< std::string >	_cgiPath;
		std::vector< std::string >	_cgiExt;

		//Upload
		std::string			_uploadDir;
};
	std::ostream& operator <<(std::ostream& os, const Location& loc);
#endif
