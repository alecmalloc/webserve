#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <map>
#include <string>
#include <ostream>

class LocationConf {
	public:
		//constructors
		LocationConf( void );
		LocationConf( const LocationConf& og );
		~LocationConf( void );

		//operator overloads
		LocationConf&	operator =( const LocationConf& og );

		//getter functions
		const std::string			getPath( void ) const;
		const std::vector< std::string >	getAllowedMethods( void ) const;
		const std::map< std::string, std::string >	getAllowedRedirects( void ) const;
		const std::string			getRootDir( void ) const;
		int					getAutoIndex( void ) const;
		const std::vector< std::string >	getIndex( void ) const;
		const std::vector< std::string >	getCgiPath( void ) const;
		const std::vector< std::string >	getCgiExt( void ) const;
		const std::string			getUploadDir( void ) const;

		//setter functions
		void	setPath( std::string );
		void	setAllowedMethod( std::string );
		void	setAllowedRedirects( std::string, std::string );
		void	setRootDir( std::string );
		void	setAutoIndex( int );
		void	setIndex( std::string );
		void	setCgiPath( std::string );
		void	setCgiExt( std::string );
		void	setUploadDir( std::string );

		//member functions
		void	checkAccess( std::string );

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
		int				_autoIndex;

		//index
		std::vector< std::string >	_index;

		//cgi
		std::vector< std::string >	_cgiPath;
		std::vector< std::string >	_cgiExt;

		//Upload
		std::string			_uploadDir;
};
	std::ostream& operator <<(std::ostream& os, const LocationConf& loc);
#endif
