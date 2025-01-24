#include "../inc/webserv.hpp"

//helper functions

static int	createEpoll( void ){
	int	epollFd;

	//create Epoll instance
	epollFd = epoll_create( MAX_EVENTS );

	//check if creation failed
	if( epollFd == -1 )
		throw( std::runtime_error( "ERROR:	Epoll:	creation failed" ) );

	//return healthy epollFd
	return( epollFd );

}

static int	setSocketAddr( struct sockaddr_in* addr, const char* ip, int port ){
	//struct for transforming ip to network style
	struct addrinfo	info, *res;

	//setting struct to zero
	std::memset( addr, 0, sizeof( *addr ) );
	std::memset( &info, 0, sizeof( info ) );

	//get addrinfo
	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_STREAM;

	if( getaddrinfo( ip, NULL, &info, &res ) != 0 ){
		return( -1 );
	}

	//set addr struct to ip and type and set port
	*addr = *( struct sockaddr_in* ) res->ai_addr;
	addr->sin_port = htons( port );
	
	//free memory from getaddrinfo
	freeaddrinfo( res );

	return( 0) ;
}

static int	setNonBlocking( int fd ){
	int	flags;

	//get old falgs
	flags = fcntl( fd, F_GETFL, 0 );

	if( flags == -1 )
		return( -1 );

	//set nonBlocking combined with old flags
	flags = fcntl( fd, F_SETFL, flags | O_NONBLOCK );

	return( flags );
}

static int	createSocket( const char* ip, int port ){
	int			newSocket;
	int			options;
	struct sockaddr_in	addr;

	//create Socket with Ip and Port with Stream option
	newSocket = socket( AF_INET, SOCK_STREAM, 0 );

	//check if creation failed and if socket at Ip:Port is already open
	if( newSocket == -1 && errno != EADDRINUSE )
		return( newSocket );

	//set sockaddr struct
	options = setSocketAddr( &addr, ip, port );
	
	if( options == -1 ){
		close( newSocket );
		return( -1 );
	}

	//bind addr to socket
	options = bind( newSocket,( struct sockaddr* ) &addr, sizeof( addr ) );
	 
	if( options == -1 ){
		close( newSocket );
		return( -1 );
	}

	//set socket to nonBlocking IO
	options = setNonBlocking( newSocket );

	if( options ==  -1 ){
		close( newSocket );
		return( -1 );
	}

	//start listening on newSocket
	options = listen( newSocket, SOMAXCONN );

	if( options == -1 ){
		close( newSocket );
		return( -1 );
	}

	return( newSocket );
}

static void	addSocketEpoll( int epollFd, int newSocket ){
	struct epoll_event	event;

	//set socket and EPOLLIN event
	event.data.fd = newSocket;
	event.events = EPOLLIN;

	//add newSocket to Epoll instance
	if( epoll_ctl( epollFd, EPOLL_CTL_ADD, newSocket, &event ) == -1 )
		throw( std::runtime_error( "EROR: Epoll:	adding socket failed" ) );
}

void	Server::_initServer( void ){
	//create epoll instance -> throws error on fails	
	_epollFd = createEpoll();

	//create temp instances for traversing
	std::vector< ServerConf >	tmpServer = _conf.getServerConfs();

	//travers thorugh each serverblock
	for( std::vector< ServerConf >::iterator it = tmpServer.begin(); \
			it != tmpServer.end(); it++ ){
	
		//create temp instance for traversing
		std::map< std::string, std::set< int > >	tempIp = it->getIpPort();

		//trvers through all Ips
		for( std::map< std::string, std::set< int > >::const_iterator it2 = \
			tempIp.begin(); it2 != tempIp.end(); it2++ ){

			//travers through each port
			for( std::set< int >::iterator it3 = it2->second.begin(); it3 != \
				it2->second.end(); it3++ ){
	
				//create sockets and add to epoll
				int	newSocket;

				newSocket = createSocket( it2->first.c_str(), *it3 );

				if( newSocket == -1 ){
					std::cerr << RED << \
						"ERROR:	Socket: failed on:	" << END \
					       	<< it2->first << ":" << *it3 << std::endl;

					continue;
				}
				//set newSocket to list with according ip and port 
				_sockets[ newSocket ] = std::make_pair( it2->first, *it3 );

				std::cout << BLUE << "INFO:	Socket:	listening on:	" \
					<< END << it2->first << ":" << *it3 << std::endl;

				//add newSocket to Epoll Instance
				addSocketEpoll( _epollFd, newSocket );
			}
		}
	}
}
