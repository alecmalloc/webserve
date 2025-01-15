#include "webserv.hpp"
#include <algorithm>

static void	cleanUp( int epoll_fd, std::vector< int > listen_fds ){
	//clean up open sockets
	for( size_t i = 0; i < listen_fds.size(); i++)
	       close( listen_fds[i] );
	close( epoll_fd );
}

static int	setNonBlocking( int tmp ){
	int	flags;

	//get flags to add non blocking to existing ones
	if( ( flags = fcntl( tmp, F_GETFL, 0 ) ) == -1 )
		return( -1 );

	//set flags in socket_fd to nonblocking for multiple connections
	if( fcntl( tmp, F_SETFL, flags | O_NONBLOCK ) == -1 )
		return( -1 );

	return( 0 );
}

static int	createEpoll( void ){
	int	epoll_fd;

	//create Epoll and throw error when failed
	if ( ( epoll_fd = epoll_create( MAX_EVENTS ) ) == -1 )
		throw( std::runtime_error( "Creating Epoll failed" ) );
	return( epoll_fd );
}

static int	createSocket( const char* ip, int port ){
	int			socket_fd;
	int			options;
	struct sockaddr_in	addr;

	//TODO::::::close sockets on error
	//create Socket with Ip:Port sending and reciving connection
	socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if( socket_fd == -1 )
		throw( std::runtime_error( "socket creation failed" ) );

	//set options to REUSE ADDRES for easier debugging and multiple ports on one ip
	options = 1;
	if( -1 == 
	setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof( options ) ) )
		throw( std::runtime_error( "Socket option adding failed" ) );
	
	//set addr stuct for socket to ip and port given in config
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;	
	//TODO:::. create inet_pton and htons
	inet_pton( AF_INET, ip, &addr.sin_addr.s_addr );
	addr.sin_port = htons( port );

	//bind socket to address
	if( bind( socket_fd, ( struct sockaddr* )&addr, sizeof( addr) ) == -1 )
		throw( std::runtime_error( "Socket bining failed" ) );

	//set socket to nonblocking so multiple conections can be connected
	if( setNonBlocking( socket_fd ) == -1 )
		throw( std::runtime_error( "Socket unblocking failed" ) );

	//start listening on socket
	if( listen( socket_fd, SOMAXCONN ) == -1 )
		throw( std::runtime_error( "Socket listening failed" ) );

	std::cout << GREEN << "Listening on: " << END << ip << ":" << port << std::endl;
	return( socket_fd );
}

static void	addSocketEpoll( int epoll_fd, int socket_fd, uint32_t events ){
	struct epoll_event	event;

	//set socket and events to epoll struct
	event.data.fd = socket_fd;
	event.events = events;

	//load event to epoll
	if( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, socket_fd, &event ) == -1 )
		throw( std::runtime_error( "Adding socket to Epoll failed" ) );
}

static void	mainLoopServer( int epoll_fd, const std::vector<int>& listen_fds ){
	int				num_events;
	int				event_fd;
	struct epoll_event		events[ MAX_EVENTS ];
	std::map< int, std::string >	client_data;

	//run loop
	while( true ){

		//wait for events to que
		if( ( num_events = epoll_wait( epoll_fd, events, MAX_EVENTS, -1 ) ) == -1 )
			throw( std::runtime_error( "Epoll wait failed" ) );

		//handle events one after another
		for( int i = 0; i < num_events; i++ ){
			
			//start with events 
			event_fd = events[i].data.fd;

			//check if event is a new connection
			if( std::find( listen_fds.begin(), listen_fds.end(), event_fd ) \
				!= listen_fds.end() ){

				//add new client to events
				struct sockaddr_in	client_addr;
				socklen_t		client_len = sizeof( client_addr );
				int			client_fd = accept( event_fd, \
					( struct sockaddr* )&client_addr, &client_len );

				//TODO:: keeping running if client failed???
				if( client_fd == -1 )
					throw( std::runtime_error( \
						"Acceptning Client failed" ) );
				
				//set Client NonBlocking and adding to epoll	
				if( setNonBlocking( client_fd ) == -1 )
					throw( std::runtime_error( \
						"Client non blocking failed" ) );
				addSocketEpoll( epoll_fd, client_fd, EPOLLIN | EPOLLET );
				std::cout << GREEN << "New Client connected: " << \
					END << client_fd << std::endl;
			}

			//Read http request
			else if ( events[i].events & EPOLLIN ){
				std::cout << BLUE << "Ready to read from: " << END << \
					event_fd << std::endl;
			}

			//write http request
			else if( events[i].events & EPOLLOUT ){
				std::cout << BLUE << "Ready to write to: " << event_fd \
					<< std::endl;
			}

			//TODO:: add other events

			//unexpected event
			else {
				std::cout << RED << "unexpected event on: " << event_fd \
					<< END << std::endl;
				close( event_fd );
			}
				
		}

	}
}

void	runServer( void ){
	std::vector< std::pair< const char*, int > >	config;
	std::vector<int>				listen_fds;
	int						epoll_fd;

	config.push_back( std::make_pair("127.0.0.1", 8080));
	config.push_back( std::make_pair("127.0.0.1", 8081));

	//create Epoll
	epoll_fd = createEpoll();	

	//add sockets for ips and ports from config
	for( size_t i = 0; i < config.size(); i++ ){
		int	listen_fd = createSocket( config[i].first, config[i].second );
		listen_fds.push_back( listen_fd );
		addSocketEpoll( epoll_fd, listen_fd, EPOLLIN );
	}

	//run main loop
	mainLoopServer( epoll_fd, listen_fds );

	//clean up sockets
	cleanUp( epoll_fd, listen_fds );
}
