#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <map>
#include <string>

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
		const std::map< int, std::string >	getAllowedRedirects( void ) const;
		const std::string			getRootDir( void ) const;
		bool				getAutoIndex( void ) const;
		const std::vector< std::string >	getIndex( void ) const;
		const std::map< std::string, std::string >	getCgi( void ) const;
		const std::string			getUploadDir( void ) const;

		//setter functions
		void	setPath( std::string );
		void	setAllowedMethod( std::string );
		void	setAllowedRedirects( int, std::string );
		void	setRootDir( std::string );
		void	setAutoIndex( bool );
		void	setIndex( std::string );
		void	setCgi( std::string, std::string );
		void	setUploadDir( std::string );

		//member functions


	private: 
		//location Path
		std::string			_path;

		//store http methiods
		std::vector< std::string >	_allowedMethods;

		//store http redirects
		std::map< int, std::string >	_allowedRedirects;

		//root directory
		std::string			_rootDir;

		//directory listing
		bool				_autoIndex;

		//index
		std::vector< std::string >	_index;

		//cgi
		std::map< std::string, std::string >	_cgi;

		//Upload
		std::string			_uploadDir;
};

#endif
