#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <string>
#include <cstddef>
#include "Location.hpp"

class Server {
	public:
		//constructors
		Server( void );
		Server( const Server& og );
		~Server( void );

		//operator overloads
		Server&	operator =( const Server& og );

		//getter functions
		const std::string			getIpPort( void ) const;
		const std::vector< std::string >	getServerNames( void ) const ;
		const std::vector< Location >		getLocations( void ) const ;
		const std::map< int, std::string >	getErrorPages( void ) const ;
		size_t 					getBodySize( void ) const ;

		//setter functions
		void	setIpPort( std::string& tmp );
		void	setServerName( const std::string& name );
		void	setLocation( Location& location );
		void	setErrorPage( int errorCode, std::string& path );
		void	setBodySize( size_t size );

		//member functions


	private: 
		//store ip address and port 
		std::string			_ipPort;

		//store server names -> stores server names as strings
		std::vector< std::string >	_serverNames;

		//store location diretives -> stores LocationConfig class
		std::vector< Location >	_locations;

		//store defaullt error pages -> error code, location
		std::map< int, std::string >	_errorPages;
		
		//limit of client body size for RFC
		size_t				_bodySize;
};

#endif
