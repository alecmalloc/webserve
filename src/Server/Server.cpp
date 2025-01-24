#include "webserv.hpp"
#include <vector>


//constructors
Server::Server( const Config& conf ) : _conf( conf ){
	_initServer();
}

Server::Server( const Server& og ) : _sockets( og._sockets ), _epollFd( og._epollFd ), \
_conf( og._conf ), _clients( og._clients ){
	;
}

Server::~Server( void ){
	//TODO:: cleanup sockets and epollfd
	;
}

//operator overlaods
Server&	Server::operator =( const Server& og ){
	if( this != &og ){
		_sockets = og._sockets;
		_epollFd = og._epollFd;
		_conf = og._conf;
		_clients = og._clients;
	}
	return( *this );
}

//getters
std::map< int, std::pair< std::string, int > >	Server::getSockets( void ) const{
	return( _sockets );
}

int			Server::getEpollFd( void ) const{
	return( _epollFd);
}

Config	Server::getConf( void ) const{
	return( _conf );
}

std::list< Client >&	Server::getClients( void ){
	return( _clients );
}

//setters
void	Server::setSockets( std::map< int, std::pair< std::string, int > > vec ){
	_sockets = vec;
}

void	Server::setEpollFd( int fd ){
	_epollFd = fd;
}

void	Server::setConf( Config conf ){
	_conf = conf;
}

void	Server::setClient( std::list< Client > clients ){
	_clients = clients;
}

//memeber fucntion
void	Server::removeClient( Client* client ){
	for( std::list< Client >::iterator it = _clients.begin(); \
			it != _clients.end(); ){
		if( &( *it ) == client ){
			_clients.erase( it );
			return;
		}
		else
			it++;
	}
}
