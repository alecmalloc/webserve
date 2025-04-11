#include "webserv.hpp"

#include <cstdlib> // For strtoul
#include <sstream> // For stringstream

static volatile bool running = true;

static void	handleSignal( int signal ){
	if( signal == SIGINT || signal == SIGQUIT ){
		running = false;
		std::cout << BLUE << "\nINFO:	Server:	Shutting down	..."  << END \
				<< std::endl;
	}
}

static void	startupSignalHandler( void ){
	struct sigaction	sa;

	//set struct to zero
	std::memset( &sa, 0, sizeof( sa ) );

	sa.sa_handler = handleSignal;
	if( sigaction( SIGQUIT, &sa, NULL ) == -1 )
		throw( std::runtime_error( "ERROR:	Server:	Start Signalhandling" ) );
	if( sigaction( SIGINT, &sa, NULL ) == -1 )
		throw( std::runtime_error( "ERROR:	Server:	Start Signalhandling" ) );
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

static int	setToEpoll( int epollFd, int clientFd ){
	 	//add ClientFd to Epoll insatance
		struct epoll_event	event;

		event.data.fd = clientFd;
		event.events = EPOLLIN | EPOLLET;

		return( epoll_ctl( epollFd, EPOLL_CTL_ADD, clientFd, &event ) );
}

static int	createNewClient( int epollFd, int fd ){
	//accept new client
	struct sockaddr_in	clientAddress;
	socklen_t		clientLen;
	int			clientFd;

	clientLen = sizeof( clientAddress );
	clientFd = accept( fd, (struct sockaddr* ) &clientAddress, &clientLen );

	if( clientFd == -1 )
		return( -1 );

	//set clientFd to nonBlocking
	if( setNonBlocking( clientFd ) == -1 ){
		close( clientFd );
		return( -1 );
	}

	if( setToEpoll( epollFd, clientFd ) == -1 ){
		close( clientFd );
		return( -1 );
	}

	return( clientFd );
}

static Client*	findClient( Server& server, struct epoll_event event ){
	//get client vector
	std::list< Client >&	clients = server.getClients();

	//go through all existing clients
	for( std::list< Client >::iterator it = clients.begin(); \
			it != clients.end(); it++ ){
		//if client already exists return
		if( it->getSocketFd() == event.data.fd )
			return( &( *it ) );

	}

	//if client doesn exist already create new one and add to epoll
	int	clientFd;
	Client	newClient;
	clientFd = createNewClient( server.getEpollFd(),  event.data.fd );

	if( clientFd == -1 ){
		std::cerr << RED << "ERROR:	Client: Connection failed" << \
				END << std::endl;
		return( NULL );
	}

	//set Event and Socket fd
	newClient.setEventFd( event.data.fd );
	newClient.setSocketFd( clientFd );

	//create new client with new fd and add to exxisting clients
	clients.push_back( newClient );

	std::cout << BLUE << "INFO:	Client: connected:	" << END << clientFd \
			<< std::endl;

	//return new client
	return( &clients.back() );
}

static void	closeClient( Server& server, Client* client ){
	//remove client fd from epoll
	epoll_ctl( server.getEpollFd(), EPOLL_CTL_DEL, client->getSocketFd(), NULL );

	//close fd
	close( client->getSocketFd() );

	//remove Client from Client list
	server.removeClient( client );
}


static ssize_t	getDefinedBodySize( std::string request ){
	
	// find content length value
	size_t contentLenPos = request.find("Content-Length:");

	//return -1 on undefined bodysize
	if (contentLenPos == std::string::npos)
		return -1;

	contentLenPos += 15;
	while (contentLenPos < request.size() && isspace(request[contentLenPos])) {
		contentLenPos++;
	}
	
	// find the end of the line
	size_t lineEnd = request.find("\r\n", contentLenPos);
	if (lineEnd == std::string::npos)
		return -1;
	
	// extract the content length value string
	std::string contentLenStr = request.substr(contentLenPos, lineEnd - contentLenPos);
	
	// Convert to integer
	size_t contentLength = 0;
	std::istringstream ss(contentLenStr);
	ss >> contentLength;

	return( contentLength );
}

static ssize_t	getRecivedBodySize( std::string request ){

	//count body length
	size_t headerEnd = request.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return( -1 );
	
	size_t bodyStart = headerEnd + 4;
	size_t bodyReceived = request.length() - bodyStart;

	// Return true if we've received the complete body
	return ( bodyReceived );
}

void print_progress( size_t progress, size_t total ) {

	// Move the cursor back to the start of the line (\r)
	std::cout << "\rProgress: " << progress << "bytes of total: " << total;

	// Flush the output buffer to ensure immediate display
	std::cout.flush();
}

static void	readFromClient( Client* client ) {

	//create Buffer and Zero it
	char	buffer[ BUFFERSIZE ];
	std::memset( buffer, '\0', BUFFERSIZE );

	//recv data from client
	int	bytesRead = recv( client->getSocketFd(), buffer, BUFFERSIZE - 1, 0 );

	//check if error occured
	if( bytesRead == -1 )
		return ;

	//store in client
	client->setContent( std::string( buffer, bytesRead ) );
	
	ssize_t	definedBodySize = getDefinedBodySize( client->getContent() );
	ssize_t	requestedBodysize = getRecivedBodySize( client->getContent() );

	//check if done reading and httprequest is valid
	if( definedBodySize > 0 ){
	
		bytesRead = recv( client->getSocketFd(), buffer, BUFFERSIZE - 1, 0 );

		while( requestedBodysize < definedBodySize && running && bytesRead > 0 ){

			client->setContent( std::string( buffer, bytesRead ) );

			std::memset( buffer, '\0', BUFFERSIZE );
			print_progress( requestedBodysize, definedBodySize );


			requestedBodysize = getRecivedBodySize( client->getContent() );

			bytesRead = recv( client->getSocketFd(), buffer, BUFFERSIZE - 1, 0 );
		}
		if( requestedBodysize >= definedBodySize )
			client->setComplete( true );
	}
	else
		client->setComplete( true );
	return;
}

static void	checkEvents( Server& server, Client* client,  struct epoll_event& event ){
	// check for error
	if( event.events & EPOLLERR ) {
		//close client
		closeClient( server, client );
		return;
	}

	// check for hanging conection
	if( event.events & EPOLLHUP || client->getClosed() ){
		closeClient( server, client );
		std::cout << BLUE << "INFO:	Client: disconnected:	" << \
			END << client->getEventFd() << std::endl;
		return;
	}

	// check if Clients send data
	if( event.events & EPOLLIN ){
		readFromClient( client );
	}

	// check for complete request instead of checking for EUP
	if ( client->getComplete() ){
		// std::cout << client->getContent() << std::endl;

		// create temp config file for request construction
		Config confTMP = server.getConf();

		// hand over content to request obj
		std::cout << "Reading request from client" << '\n';
		HttpRequest request(confTMP);
		const std::string request_str = client->getContent();
		request.handleRequest(request_str);
		std::cout << request << '\n';
		
		// Get server configs
		std::vector<ServerConf> serverTMPConf = confTMP.getServerConfs();

		// Initialize server conf pointer
		ServerConf* selectedServerConf = NULL;

		// Get the Host header from the request
		std::string host = request.getHostname();

		// Match server based on host/server_name
		for (size_t i = 0; i < serverTMPConf.size(); i++) {
			// check server names
			const std::vector<std::string>& serverNames = serverTMPConf[i].getServerConfNames();
			if (std::find(serverNames.begin(), serverNames.end(), host) != serverNames.end()) {
				selectedServerConf = &serverTMPConf[i];
				break;
			}

			// check server ips
			const std::map<std::string, std::set<int> >& ipPorts = serverTMPConf[i].getIpPort();
			if (ipPorts.find(host) != ipPorts.end()) {
				selectedServerConf = &serverTMPConf[i];
				break;
			}
		}

		// Use the first server as default if no match
		if (!selectedServerConf && !serverTMPConf.empty()) {
			selectedServerConf = &serverTMPConf[0];
		}

		// Before creating response, validate
		if (!selectedServerConf) {
			std::cerr << "CRITICAL ERROR: No server configuration available" << std::endl;
			// needs to be a 500 internal server error
			request.setResponseCode(500);
		}

		// Create response with the selected server
		Response response(request, selectedServerConf);

		// write response to socket
		write(client->getSocketFd(), response.getHttpResponse().c_str(), response.getHttpResponse().size());

		client->clearContent();
		client->setComplete( false );

		if( std::strstr ( client->getContent().c_str(), "Connection: close" ) )
			client->setClosed( true );
	}

	//check for error
	if( event.events & EPOLLERR || client->getError() ) {
		//close client
		closeClient( server, client );
		std::cerr << RED << "ERROR:	Client: fd:	" << END << \
			client->getEventFd() << strerror( errno ) << std::endl;
		return;
	}

	//check for hanging conection
	if( event.events & EPOLLHUP || client->getClosed() ){
		std::cout << BLUE << "INFO:	Client: disconnected:	" << \
			END << client->getEventFd() << std::endl;
		closeClient( server, client );
		return;
	}
			
}

static void	mainLoop( Server& server ){
	//epoll control
	int			activeEvents;
	struct epoll_event	events[ MAX_EVENTS ];

	//saving open clients
	std::vector< Client >	clients;

	while( running ){
		//register new events with epoll wait
		activeEvents = epoll_wait( server.getEpollFd(), events, MAX_EVENTS, -1 );

		if( activeEvents == -1 && errno != EINTR )
			throw( std::runtime_error( "ERROR:	Epoll: wait failed" ) );

		//handle all events
		for( int i = 0; i < activeEvents; i++ ){

			//check if client already exists
			Client*	tmp = findClient( server, events[i] );

			//check if client was found
			if( !tmp )
				continue;

			//check for event
			checkEvents( server, tmp, events[i] );
		}

	}

}

void	Server::runServer( void ){

	try{

		//startup signal handler
		startupSignalHandler();

		//run main loop
		mainLoop( *this );

	}
	catch( std::exception& e ){
		std::cerr << RED << e.what() << END << std::endl;
		return;
	}

}
