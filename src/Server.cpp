#include "webserv.hpp"
#include <algorithm>

#define BUFFERSIZE 100

//global variable for signalhandling
static volatile bool running = true;

//handle on SIGINT
static void	handleSignal( int signal ){
	if( signal == SIGINT ) {
		running = false;
		std::cout << BLUE << "\nShutting down Server ... ... ... " << END << std::endl;
	}

}

//setup Signalhandler
static void	setupSignalHandling( void ){
	struct sigaction	sa;

	memset( &sa, 0, sizeof( sa ) );
	sa.sa_handler = handleSignal;
	sigaction( SIGINT, &sa, NULL );
}

//cleanup sockets and epoll
static void	cleanUp( int epoll_fd, std::vector< int > listen_fds ){
	for( size_t i = 0; i < listen_fds.size(); i++)
	       close( listen_fds[i] );
	close( epoll_fd );
}

//set non Blocking to fds
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

	//if something with sockets failes -> closing that socket and go on

	//create Socket with Ip:Port sending and reciving connection
	socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if( socket_fd == -1 && errno != EADDRINUSE ){
		std::cerr << RED << "Creating Socket: " << socket_fd << " failed" << \
			"on ip: " << ip << " with port: " << port << " because of: " << \
			strerror( errno ) << BLUE << "\nCarry on without that Socket" << \
			END << std::endl;
		return( -1 );
	}

	//set options to REUSE ADDRES for easier debugging and multiple ports on one ip
	options = 1;
	if( setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, &options, sizeof( options ) ) == -1 ){
		std::cerr << RED << "Setting options on Socket: " << socket_fd << " failed" << \
			"on ip: " << ip << " with port: " << port << " because of: " << \
			strerror( errno ) << BLUE << "\nCarry on without that Socket" << \
			END << std::endl;
		close( socket_fd );
		return( -1 );
	}
	
	//set addr stuct for socket to ip and port given in config
	memset( &addr, 0, sizeof( addr ) );
	addr.sin_family = AF_INET;	
	//TODO:::. create inet_pton and htons
	inet_pton( AF_INET, ip, &addr.sin_addr.s_addr );
	addr.sin_port = htons( port );

	//bind socket to address
	if( bind( socket_fd, ( struct sockaddr* )&addr, sizeof( addr) ) == -1 ){
		std::cerr << RED << "Binding Socket: " << socket_fd << " failed" << \
			"on ip: " << ip << " with port: " << port << " because of: " << \
			strerror( errno ) << BLUE << "\nCarry on without that Socket" << \
			END << std::endl;
		close( socket_fd );
		return( -1 );
	}

	//set socket to nonblocking so multiple conections can be connected
	if( setNonBlocking( socket_fd ) == -1 ){
		std::cerr << RED << "Set Socket: " << socket_fd << " to non Blocking failed" << \
			"on ip: " << ip << " with port: " << port << " because of: " << \
			strerror( errno ) << BLUE << "\nCarry on without that Socket" << \
			END << std::endl;
		close( socket_fd );
		return( -1 );
	}

	//start listening on socket
	if( listen( socket_fd, SOMAXCONN ) == -1 ){
		std::cerr << RED << "Listening to Socket: " << socket_fd << " failed" << \
			"on ip: " << ip << " with port: " << port << " because of: " << \
			strerror( errno ) << BLUE << "\nCarry on without that Socket" << \
			END << std::endl;
		close( socket_fd );
		return( -1 );
	}

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

static void	mainLoopServer( Config& conf, int epoll_fd, const std::vector<int>& listen_fds ){
	int				num_events;
	int				event_fd;
	struct epoll_event		events[ MAX_EVENTS ];
	std::map< int, std::string >	client_data;

	//TODO: give config file to linus and alec
	(void) conf;

	//run loop
	while( running ){

		//wait for events to que
		if( ( ( num_events = epoll_wait( epoll_fd, events, MAX_EVENTS, -1 ) ) \
				== -1 ) && ( errno != EINTR ) )
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

				if( client_fd == -1 ){
					std::cerr << RED << "Client Conection failed" << \
						END << std::endl;
					continue;
				}
				
				//set Client NonBlocking and adding to epoll	
				if( setNonBlocking( client_fd ) == -1 ){
					std::cerr << RED << "Setting Client to non Blocking failed" \
					       << END << std::endl;	
					close( client_fd );
					continue;
				}
				addSocketEpoll( epoll_fd, client_fd, EPOLLIN | EPOLLET );
				std::cout << GREEN << "New Client connected: " << \
					END << client_fd << std::endl;
			}

			//Read http request
			if ( events[i].events & EPOLLIN ){
				std::cout << BLUE << "Ready to read from: " << END << \
					event_fd << std::endl;

				char	buffer[ BUFFERSIZE ];
				int	bytes_read;

				//read sended stuff 
				//TODO:: moving to http request handler??
				do{
					bytes_read = \
						read( event_fd, buffer, BUFFERSIZE - 1);
					if( bytes_read > 0 ){
						buffer[ BUFFERSIZE ] = '\0';
						std::cout << buffer;
					}
				} while( bytes_read > 0 || \
						( bytes_read == -1 && errno == EINTR ) );
				//TODO: ending conections handeld by http request handler??
					// Alec TODO: functioncall(eventfd, Config& conf)
			}

			//write http request
			if( events[i].events & EPOLLOUT ){
				std::cout << BLUE << "Ready to write to: " << event_fd \
					<< std::endl;
				//TODO: ending conections handeld by http request handler??
			}
			//TODO:: add other events
		}

	}
}


static void	setupServer( Config& conf, int& epoll_fd, std::vector< int >& listen_fds ){

	//store servers and ips
	std::vector< ServerConf >	server = conf.getServerConfs();

	//create Epoll
	epoll_fd = createEpoll();	

	//travers through each port of ips from each server block
	for( std::vector< ServerConf >::const_iterator it = server.begin(); \
		it != server.end(); it++ ){
		std::map< std::string, std::set< int > >	ip = it->getIpPort();
		for( std::map< std::string, std::set< int > >::const_iterator it2 = ip.begin(); \
			it2 != ip.end(); it2++ ){	
			std::set< int >	port = it2->second;
			for( std::set< int >::const_iterator it3 = port.begin(); \
					it3 != port.end(); it3++ ){

				//add sockets for ips and ports from config
				int	listen_fd = createSocket( it2->first.c_str(), *it3 );
				if( listen_fd > 0 ){
					//add to list of open fds
					listen_fds.push_back( listen_fd );

					//add to epoll
					addSocketEpoll( epoll_fd, listen_fd, EPOLLIN );
				}
			}
		}
	}
}

void	runServer( Config& conf ){
	std::vector<int>				listen_fds;
	int						epoll_fd;

	//start signal hander
	setupSignalHandling();

	//setup Servers
	setupServer( conf, epoll_fd, listen_fds );

	//run main loop
	mainLoopServer( conf, epoll_fd, listen_fds );

	//clean up sockets
	cleanUp( epoll_fd, listen_fds );
}
