#ifndef SERVER_HPP
#define SERVER_HPP

#include <ostream>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>
#include <cstddef>
#include <netdb.h>

#include "Location.hpp"
#include "Config.hpp"
#include "Client.hpp"

class	Server{
	public:
		//constructors
		Server( const Config& );
		Server( const Server& );
		~Server( void );

		//operator overloads
		Server&	operator =( const Server& );

		//getters
		std::map< int, std::pair< std::string, int > >	getSockets( void ) const;
		int			getEpollFd( void ) const;
		Config			getConf( void ) const;
		std::list< Client >&	getClients( void );

		//setters
		void	setSockets( std::map< int, std::pair< std::string, int > > );
		void	setEpollFd( int );
		void	setConf( Config );
		void	setClient( std::list<Client > );

		//member functions
		void		runServer( void );
		void		removeClient( Client* );

	private:
		//stores sockets fds, their according ip and port
		std::map< int, std::pair< std::string, int > >		_sockets;

		//stores epollfd
		int							_epollFd;

		//stores config file
		Config							_conf;

		//stores connected clients
		std::list< Client >					_clients;

		//private member functions
		void		_initServer( void );				
};

std::ostream&	operator <<( std::ostream& os, const ServerConf& server );
#endif
