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

static std::string	stripQueryParams(const std::string& uri) {

	size_t	pos = uri.find('?');

	if( pos != std::string::npos )
		return uri.substr(0, pos);

	return uri;
}

static std::string	extractQueryString(const std::string& uri) {
	size_t pos = uri.find('?');
	if (pos != std::string::npos) {
		return uri.substr(pos + 1);
	}
	return "";
}

static void	checkFile( Response& resp, std::string& interpreter ){

	//vectors with stored cgi parameters
	std::vector<std::string>	ext = resp.getLocationConf().getCgiExt();
	std::vector<std::string>	path = resp.getLocationConf().getCgiPath();

	//store request
	const HttpRequest&		req = resp.getHttpRequest();

	std::string			uri = stripQueryParams(req.getUri());
	std::string			end;
	std::string::size_type		dot = uri.rfind( '.', uri.npos );

	//return if no ending is found eg ".pl"
	if( dot == std::string::npos ){
		throw( 404 );
	}

	end = uri.substr( dot, uri.npos );
	std::vector< std::string >::iterator it = std::find( ext.begin(), ext.end(), end );

	//return if ending is not inside cgiExt
	if( it == ext.end() ){
		throw( 404 );
	}

	std::string fullPath = resp.getLocationConf().getRootDir().empty() ? \
			resp.getServerConf().getRootDir() : resp.getLocationConf().getRootDir() + uri;

	//check if file is accesible
	if( access( fullPath.c_str(), F_OK ) == -1 )
		throw( 404 );

	//check if file is executealbe
	if( access( fullPath.c_str(), X_OK ) == -1 )
		throw( 403 );

	std::vector<std::string>::iterator	extIt = ext.begin();
	std::vector<std::string>::iterator	pathIt = path.begin();

	while( extIt != ext.end() &&  pathIt != path.end() ){
		if( extIt == it ){
			interpreter = *pathIt;
			break;
		}
		extIt++;
		pathIt++;
	}

	if( interpreter.empty() )
		throw( 500 );
}

static void	setEnv( Response& resp, char*** env ){

	//HttpRequest
	const HttpRequest&	req = resp.getHttpRequest();

	//get SERVER_IP and SERVER_PORT for env
	std::string	ip = resp.getServerName();
	std::string	port = toString( req.getPort() );

	// Extract from Host header if available
	if( req.getHeaders().count("Host") ) {
		std::string host = req.getHeaders().at( "Host" )[0];
		size_t colonPos = host.find(':');
		if( colonPos != std::string::npos ){
			ip = host.substr(0, colonPos);
			port = host.substr(colonPos+1);
		}
		else 
			ip = host;
	}

	//set env via strings 
	// Extract query string separately
	std::string queryString = extractQueryString( req.getUri() );

	std::map< std::string, std::string>	tmpEnv;

	tmpEnv["REQUEST_METHOD"] = req.getMethod().empty() ? "GET" : req.getMethod();
	tmpEnv["QUERY_STRING"] = queryString;
	tmpEnv["SCRIPT_NAME"] = req.getUrl().empty() ? "/index.cgi" : req.getUrl();
	tmpEnv["SCRIPT_FILENAME"] = req.getUrl().empty() ? "/index.cgi" : req.getUrl();
	tmpEnv["CONTENT_LENGTH"] = req.getMethod()== "POST" ? toString( req.getBody().length() ) : "0";
	tmpEnv["CONTENT_TYPE"] = req.getHeaders().count( "Content-Type" ) ? \
		req.getHeaders().at( "Content-Type" )[0] : "text/plain";
	tmpEnv["SERVER_PROTOCOL"] = req.getVersion().empty() ? "HTTP/1.1" : req.getVersion();
	tmpEnv["REDIRECT_STATUS"] = "200";
	tmpEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
	tmpEnv["PATH_INFO"] = req.getUri().empty() ? "" : req.getUri();
	tmpEnv["SERVER_NAME"] = req.getHeaders().count("Host") ? \
		req.getHeaders().at("Host")[0] : ip + ":" + port;
	tmpEnv["SERVER_PORT"] = port;
	tmpEnv["REMOTE_ADDR"] = ip;
	tmpEnv["HOST"] = resp.getServerName();
	tmpEnv["REQUEST_URI"] = req.getHeaders().count("Host") ? \
		req.getHeaders().at("Host")[0] + req.getUri() : ip + ":" + port + req.getUri();

	//transfer headers to be HTTP_...
	const std::map<std::string, std::vector< std::string> >&	headers = req.getHeaders();

	for( std::map<std::string, std::vector< std::string> >::const_iterator it = headers.begin(); \
		it != headers.end(); it++ ){

		std::string key = "HTTP_" + it->first;
		std::replace(key.begin(), key.end(), '-', '_');
		tmpEnv[ key ] = it->second.empty() ? "" : it->second[0];
	}

	//transfer map into char**
	*env = new char*[ tmpEnv.size() + 1 ];

	int index = 0;
	for( std::map< std::string, std::string >::iterator it = tmpEnv.begin(); \
			it != tmpEnv.end(); it++ ){

		std::string	entry = it->first + "=" + it->second;
		(*env)[index] = new char[entry.length() + 1];
		(*env)[index] = std::strcpy( (*env)[index], entry.c_str() );
		index++;
	}

	(*env)[index] = NULL;
}

static int	childProcess( Response& resp, int* inputPipe, int* outputPipe, \
		std::string& interpreter ){

	const HttpRequest&	req = resp.getHttpRequest();

	//dup input to pipe nad output to pipe for connection to parent
	if( dup2( inputPipe[0], STDIN_FILENO ) == -1 ){

		std::cerr << RED << "ERROR: Dup failed" << END << std::endl;
		closePipe( inputPipe );
		closePipe( outputPipe );
		throw( 500 );
	}
	if( dup2( outputPipe[1], STDOUT_FILENO ) == -1 ){

		std::cerr << RED << "ERROR: Dup failed" << END << std::endl;
		closePipe( inputPipe );
		closePipe( outputPipe );
		throw( 500 );
	}

	//create env for execve
	char	**env;
	setEnv( resp, &env );

	//close unused ends
	close( inputPipe[1] );
	close( outputPipe[0] );

	// Construct the full file path by combining the root directory with the URI
	std::string		rootDir = resp.getServerConf().getRootDir();
	std::string		uri = stripQueryParams(req.getUri());
	std::string		scriptPath = rootDir + uri;

	// Pass the clean path to execve
	const std::string inter = interpreter;
	char* args[] = { const_cast<char*>( inter.c_str() ), const_cast<char*>( scriptPath.c_str() ), NULL };

	//execute
	execve( inter.c_str(), args, env );

	//error 
	close( inputPipe[0] );
	close( outputPipe[1] );

	for( int i = 0; env[i]; i++ )
		delete[] env[i];

	delete[] env;
	throw( 500 );
}

// Function to get the body of the response by skipping headers
static std::string	getBody( const std::string& response ){

	// If response is empty, return empty string
	if( response.empty() )
		return "";

	// Search for the standard HTTP header/body separator from the end
	size_t	bodyStart = 0;

	// Look for the last header separator
	size_t	headerSep = response.rfind( "\r\n\r\n" );

	//if found standard HTTP header separator
	if( headerSep != std::string::npos )
		bodyStart = headerSep + 4;
	else{

		// Try Unix-style separator
		headerSep = response.rfind("\n\n");
		if( headerSep != std::string::npos )
			bodyStart = headerSep + 2;
		else
			return response;
	}

	// Return everything after the separator
	return( response.substr( bodyStart ) );
}

static void	parentProcess( Response& resp, int* inputPipe, int* outputPipe, pid_t pid ){

	HttpRequest&	req = resp.getHttpRequest();

	//status for childs
	int			status;

	//close unused pipe ends
	close( inputPipe[0] );
	close( outputPipe[1] );

	//send body to process if needed
	if( req.getMethod() == "POST" ){

		size_t		bytesWritten = 0;
		size_t 		totalBytes = req.getBody().size();
		std::string	data = req.getBody();

		while( bytesWritten < totalBytes ) {
			ssize_t result;
			result = write( inputPipe[1], data.c_str() + bytesWritten, \
					totalBytes - bytesWritten );

			// Handle error
			if( result <= 0 )
				throw( 500 );

			bytesWritten += result;
		}

		// Add a small delay to ensure data is transferred before closing pipe
		usleep( 5000 );  // 1ms delay
	}


	//close reaminig inputpipe so process dosent wait
	close( inputPipe[1] );

	//wait for child for TIMEOUT_TIME amount and kill if not working
	struct pollfd	pfd;
	int		pollRet;
	pfd.fd = outputPipe[0];
	pfd.events = POLLIN;	


	//read from open pipe into buffer and store in response string
	char		buffer[ BUFFERSIZE ];
	std::string	response;
	size_t		bytesRead;

	time_t	startTime = time( NULL );
	std::memset( buffer, '\0', BUFFERSIZE );

	while( true ){

		//wait for child to write to fd
		pollRet = poll( &pfd, 1, TIMEOUT_TIME );

		if( pollRet == -1 ){
			std::cerr << RED << "ERROR: Poll: " << strerror( errno ) << END << std::endl;
			close( outputPipe[0] );
			kill( pid, SIGKILL );
			waitpid( pid, &status, 0 );
			throw( 500 );
		}

		//if return == TIMEOUT
		else if( pollRet == 0 || ( startTime - time( NULL ) ) > TIMEOUT_TIME ){
			//std::cout << BLUE << "INFO: Cgi: Timeout" << END << '\n';
			close( outputPipe[0] );
			kill( pid, SIGKILL );
			waitpid( pid, &status, 0 );
			throw( 504 );
		}

		else{
			bytesRead = read( outputPipe[0], buffer, BUFFERSIZE - 1 );

			if( bytesRead > MAX_BYTES ){
				std::cerr << "CGI: Limit exceeded" << std::endl;
				kill( pid, SIGKILL );
				throw( 500 );
			}

			if( bytesRead > 0 ){
				response += std::string( buffer, BUFFERSIZE );
				std::memset( buffer, '\0', BUFFERSIZE );
				startTime = time( NULL );
			}
			else if( bytesRead == 0 )
				break;
		}
	}

	response = getBody( response );
	response += CSS_GLOBAL;

	resp.setBody( response );

	//close final pipe
	close( outputPipe[0] );

	//wait and get status
	waitpid( pid, &status, 0 );

	if( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 )
		throw( 500 );
}

static void	checkBody( Response& resp, std::string body ){

	size_t maxBodySize = resp.getLocationConf().getBodySizeInitilized() ? \
		resp.getLocationConf().getBodySizeInitilized() : resp.getServerConf().getBodySize();

	if( body.size() > maxBodySize ){
		std::cerr << RED << "ERROR: Cgi: POST TO BIG" << END << std::endl;
		throw( 413 );
	}
	while( !body.empty() && ( body[body.size()-1] == '\n' || body[body.size()-1] == '\r' ) )
		body.erase( body.size() -1 );

	resp.getHttpRequest().setBody( body );
}

void	handleCgi( Response& resp ){

	//Cgi stuff
	int		inputPipe[2], outputPipe[2];
	pid_t		pid;

	//HttpRequest stuff
	HttpRequest&	req = resp.getHttpRequest();

	//check and clean body
	if( req.getMethod() == "POST" )
		checkBody( resp, req.getBody() );

	//check file ending and access to it  and store interpreter for executuing
	std::string	interpreter;
	checkFile( resp, interpreter );
	
	//open pipes for sending data
	if( pipe( inputPipe ) < 0 ){
		std::cerr << RED << "ERROR: Cgi: Pipe" << END << std::endl;
		throw( 500 );
	}

	if( pipe( outputPipe ) < 0 ){
		std::cerr << RED << "ERROR: Cgi: Pipe" << END << std::endl;
		closePipe( inputPipe );
		throw( 500 );
	}

	//fork for cgi execution
	pid = fork();


	//handle fork error
	if( pid < 0 ){
		std::cerr << RED << "ERROR: Cgi: Fork" << END << std::endl;
		closePipe( inputPipe );
		closePipe( outputPipe );
		throw(500);
	}

	//handle child
	if( pid == 0 )
		childProcess( resp, inputPipe, outputPipe, interpreter );

	//handle parent
	else
		parentProcess( resp, inputPipe, outputPipe, pid );
}
