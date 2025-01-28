#include "webserv.hpp"
#include <sstream>
#include <string>

static void	closePipe( int* fd ){
	close( fd[0] );
	close( fd[1] );
}

static std::string	toString( size_t i ){
	std::stringstream	ss;

	ss << i;
	return( ss.str() );
}

//get correct location block and theire ext and paths
static int	getVectors( ServerConf server, std::vector< std::string >& ext, \
			std::vector< std::string >& path, std::string uri ){

	//stores all location files from this server block
	const std::vector< LocationConf >&	locations = server.getLocationConfs();

	//get uri path
	std::string	uriPath;
	std::string::size_type		slash = uri.rfind( '/', uri.npos );

	//return if no path is foun"
	if( slash == std::string::npos )
		return( -1 );

	uriPath = uri.substr( 0, slash );

	for( std::vector< LocationConf >::const_iterator it = locations.begin(); it != \
			locations.end(); it++ ){
		std::string dir = it->getRootDir().empty() ? ( server.getRootDir() + 
				it->getPath() ): ( it->getRootDir() + it->getPath() );
		if( dir == uriPath ){
			ext = it->getCgiExt();
			path = it->getCgiPath();
			return( 0 );
		}
	}
	return( -1 );
}
static int	checkFile( HttpRequest& req, std::string& interpreter ){
	//vectors with stored cgi parameters
	std::vector< std::string > ext;
	std::vector< std::string > path;

	//get vectors
	if( getVectors( req.getServer(), ext, path, req.getUri() ) == -1 )
		return( -1 );

	//get ending and compare with cgi ext
	std::string			end;
	std::string			uri = req.getUri();
	std::string::size_type		dot = uri.rfind( '.', uri.npos );

	//return if no ending is found ".cgi"
	if( dot == std::string::npos )
		return( -1 );

	end = uri.substr( dot, uri.npos );
	std::vector< std::string >::iterator it = std::find( ext.begin(), ext.end(), end );

	//return if ending is not inside cgiExt
	if( it == ext.end() )
		return( -1 );

	//check if file is accesible
	if( access( uri.c_str(), X_OK ) == -1 )
		return( -1 );

	std::vector< std::string >::iterator extIt = ext.begin();
	std::vector< std::string >::iterator pathIt = path.begin();

	while( extIt != ext.end() &&  pathIt != path.end() ){
		if( extIt == it ){
			interpreter = *pathIt;
			break;
		}
		extIt++;
		pathIt++;
	}
	if( interpreter.empty() )
		return( -1 );
	return( 0 );
}

// COMMENTED OUT BECAUSE WE DONT NEED AND REQUEST DOESNT HAVE FD ANYMORE
// static void	setIpPort( HttpRequest& req, std::string& ip, std::string& port ){
// 	struct sockaddr_in	serverAddr;
// 	socklen_t		addrLen = sizeof( serverAddr );	

// 	if( getsockname( req.getFd(), ( struct sockaddr* ) &serverAddr, &addrLen ) == 0){
// 		ip = inet_ntoa( serverAddr.sin_addr );
// 		port = toString( ntohs( serverAddr.sin_port ) );
// 	}
// 	else {
// 		ip = "127.0.0.1";
// 		port = "80";
// 	}
// }

//TODO: check if other env variables needed?
static void	setEnv( HttpRequest& req, char*** env ){
	//get SERVER_IP and SERVER_PORT for env
	std::string ip, port;

	// setIpPort( req, ip, port );


	//set env via strings 
	std::map< std::string, std::string > tmpEnv;

	tmpEnv[ "REQUEST_METHOD" ]	= req.getMethod().empty() ? "GET" : req.getMethod();
	tmpEnv[ "QUERY_STRING" ]  	= req.getUri().empty() ? "" : req.getUri();
	tmpEnv[ "SCRIPT_NAME" ]   	= req.getUrl().empty() ? \
						"/index.cgi" : req.getUrl();
	tmpEnv[ "SCRIPT_FILENAME" ]   	= req.getUrl().empty() ? \
						"/index.cgi" : req.getUrl();
	tmpEnv[ "CONTENT_LENGTH" ]	= ( req.getMethod()== "POST" ) ? \
						toString( req.getBody().length() ) : "0";
	tmpEnv[ "CONTEN_TYPE" ]   	= req.getHeaders().count( "Content-Type" ) ? \
						req.getHeaders().at( "Content-Type" )[0] : \
						"text/plain";
	tmpEnv[ "SERVER_NAME" ]   	= req.getHeaders().count( "Host" ) ? \
						req.getHeaders().at( "Host" )[0] : \
						"localhost";
	tmpEnv[ "SERVER_PORT" ]   	= port;
	tmpEnv[ "SERVER_PROTOCOL" ]  	= req.getVersion().empty() ? "HTTP/1.1" :
						req.getVersion();
	tmpEnv[ "REMOTE_ADDR" ]   	= ip;
	tmpEnv[ "REDIRECT_STATUS" ]	= "200";
	tmpEnv[ "GATEWAY_INTERFACE" ]	= "CGI/1.1";
	tmpEnv[ "PATH_INFO" ]		= req.getUri().empty() ? "" : req.getUri();
	tmpEnv[ "REQUEST_URI" ]		= req.getUri().empty() ? "" : req.getUrl().empty() \
						? "" : req.getUrl();
	
	//transfer headers to be HTTP_...
	const std::map< std::string, std::vector< std::string > >& headers = req.getHeaders();
	for( std::map< std::string, std::vector< std::string > >::const_iterator it = \
			headers.begin(); it != headers.end(); it++ ){
		std::string key = "HTTP_" + it->first;
		key.replace( key.begin(), key.end(), '-', '_' );
		tmpEnv[ key ] = it->second.empty() ? "" : it->second[0];
	}

	//transfer map into char**
	*env = new char*[ tmpEnv.size() + 1 ];

	int index = 0;
	for( std::map< std::string, std::string >::iterator it = tmpEnv.begin(); \
			it != tmpEnv.end(); it++ ){
		std::string	entry = it->first + "=" + it->second;
		( *env )[ index ] = new char[ entry.length() + 1 ];
		( *env )[ index ] = std::strcpy( ( *env )[ index ], entry.c_str() );
		index++;
	}
	( *env )[ index ] = NULL;
}

static int	childProcess( HttpRequest& req, int* inputPipe, int* outputPipe, \
	       		std::string& interpreter ){
	//dup input to pipe nad output to pipe for connection to parent
	if( dup2( inputPipe[0], STDIN_FILENO ) == -1 ){
		std::cerr << RED << "ERROR: Dup failed" << END << std::endl;
		closePipe( inputPipe );
		closePipe( outputPipe );
		return( -1 );
	}
	if( dup2( outputPipe[1], STDOUT_FILENO ) == -1 ){
		std::cerr << RED << "ERROR: Dup failed" << END << std::endl;
		closePipe( inputPipe );
		closePipe( outputPipe );
		return( -1 );
	}

	//create env for execve
	char	**env;
	setEnv( req, &env );

	//close unused ends
	close( inputPipe[1] );
	close( outputPipe[0] );

	//set args for execve 
	const std::string path = req.getUrl().c_str();
	const std::string inter = interpreter.c_str();
	char* args[] = { const_cast< char* >( inter.c_str() ), \
				const_cast< char* >( path.c_str() ), NULL };

	//execute
	execve( inter.c_str(), args, env );

	//error 
	close( inputPipe[0] );
	close( outputPipe[1] );
	for( int i = 0; env[i]; i++ ){
		delete[] env[i];
	}
	delete[] env;

	return( -1 );
}

static int	parentProcess( HttpRequest& req, int* inputPipe, int* outputPipe, pid_t pid ){
	//status for childs
	int	status;

	//close unused pipe ends
	close( inputPipe[0] );
	close( outputPipe[1] );

	//send body to process if needed
	if( req.getMethod() == "POST" )
		write( inputPipe[1], req.getBody().c_str(), req.getBody().size() );
	
	//close reaminig inputpipe so process dosent wait
	close( inputPipe[1] );

	//wait for child for TIMEOUT_TIME amount and kill if not working
	struct pollfd	pfd;
	int		pollRet;
	pfd.fd 		= outputPipe[0];
	pfd.events 	= POLLIN;	

	//wait for child to write to fd
	pollRet = poll( &pfd, 1, TIMEOUT_TIME );

	if( pollRet == -1 ){
		std::cerr << RED << "ERROR: Poll: " << strerror( errno ) << END << std::endl;
		close( outputPipe[0] );
		kill( pid, SIGKILL );
		waitpid( pid, &status, 0 );
		return( -1 );
	}

	//if return == TIMEOUT
	if( pollRet == 0 ){
		std::cout << BLUE << "INFO: Cgi: Timeout" << END << std::endl;
		close( outputPipe[0] );
		kill( pid, SIGKILL );
		waitpid( pid, &status, 0 );
		return( -1 );
	}

	//read from open pipe into buffer and store in response string
	char		buffer[ BUFFERSIZE ];
	std::memset( buffer, '\0', BUFFERSIZE );
	std::string	response;
	size_t		bytesRead;

	while( ( bytesRead = read( outputPipe[0], buffer, BUFFERSIZE - 1 ) ) > 0){
		response += buffer;
		std::memset( buffer, '\0', BUFFERSIZE );
	}

	//print response for testing
	std::cout << "CGI output:\n" <<  response << std::endl;
	
	//close final pipe
	close( outputPipe[0] );

	//wait and get status
	waitpid( pid, &status, 0 );

	if( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 )
		return( -1 );
	return( 0 );
}

int	handleCgi( HttpRequest& req ){

	int	inputPipe[2], outputPipe[2];
	pid_t	pid;

	//check file ending and access to it  and store interpreter for executuing
	std::string	interpreter;
	if( checkFile( req, interpreter ) == -1 ){
		std::cerr << RED << "ERROR: Cgi: not found " << req.getUri() << END << \
			std::endl;
		return( -1 );
	}

	//open pipes for sending data
	if( pipe( inputPipe ) < 0 ){
		std::cerr << RED << "ERROR: Cgi: Pipe" << END << std::endl;
		return( -1 );
	}

	if( pipe( outputPipe ) < 0 ){
		std::cerr << RED << "ERROR: Cgi: Pipe" << END << std::endl;
		closePipe( inputPipe );
		return( -1 );
	}

	//fork for cgi execution
	pid = fork();


	//handle fork error
	if( pid < 0 ){
		std::cerr << RED << "ERROR: Cgi: Fork" << END << std::endl;
		closePipe( inputPipe );
		closePipe( outputPipe );
		return( -1 );
	}

	//handle child
	if( pid == 0 )
		return( childProcess( req, inputPipe, outputPipe, interpreter ) );

	//handle parent
	else
		return( parentProcess( req, inputPipe, outputPipe, pid ) );

}
