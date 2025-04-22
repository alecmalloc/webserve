#ifndef SERVERCONF_HPP
# define SERVERCONF_HPP

#include <vector>
#include <set>
#include <map>
#include <string>
#include <cstddef>

#include "Location.hpp"

class ServerConf {
	public:
		//constructors
		ServerConf( void );
		ServerConf( const ServerConf& og );
		~ServerConf( void );

		//operator overloads
		ServerConf&		operator =( const ServerConf& og );

		//getter functions
		const std::map< std::string, std::set< int > >	getIpPort( void ) const;
		const std::vector< std::string >		getServerConfNames( void ) const ;
		const std::vector< LocationConf >		getLocationConfs( void ) const ;
		const std::map< int, std::string >		getErrorPages( void ) const ;
		size_t 						getBodySize( void ) const ;
		const std::string				getRootDir( void ) const;
		bool						getAutoIndex( void ) const;
		const std::vector< std::string >		getIndex( void ) const;
		bool						getChunkedTransfer( void ) const;
		size_t						getChunkSize( void ) const;
		const std::map< std::string, std::string >	getAllowedRedirects( void ) const;

		//setter functions
		void	setIpPort( std::string ip, int port );
		void	setServerConfName( const std::string name );
		void	setLocationConf( LocationConf location );
		void	setErrorPage( int errorCode, std::string path );
		void	setBodySize( size_t size );
		void	setRootDir( std::string tmp );
		void	setAutoIndex( bool status );
		void	setIndex( std::string );
		void	setChunkedTransfer( bool chunked );
 		void	setChunkSize( size_t size );
		void	setAllowedRedirects( std::string, std::string );

		//member functions
		void	checkAccess( void );

	private: 
		//store ip address and port 
		std::map< std::string, std::set< int > >		_ipPort;

		//store server names -> stores server names as strings
		std::vector< std::string >	_serverNames;

		//store location diretives -> stores LocationConfConfig class
		std::vector< LocationConf >	_locations;

		//store defaullt error pages -> error code, location
		std::map< int, std::string >	_errorPages;

		//limit of client body size for RFC
		size_t				_bodySize;

		//root directory -> dir
		std::string			_rootDir;

		//store indexes
		std::vector< std::string >	_index;
	
		//directory listing
		bool				_autoIndex;

		//flag to indicate if chunked transfer is enabled
	        bool				_chunkedTransfer;
	
	        //size of each chunk
		size_t				_chunkSize;
		
		//store http redirects
		std::map< std::string, std::string >	_allowedRedirects;
};

#endif
