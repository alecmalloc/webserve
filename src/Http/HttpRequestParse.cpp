#include "webserv.hpp"

void	HttpRequest::parseHeaders( const std::string& rawRequest ){

	// split request data into lines
	std::stringstream	ss( rawRequest );
	std::string		line;

	// Parse request line (METHOD URI HTTP/VERSION)
	if( !std::getline( ss, line ) ){
		setResponseCode( 400 );
		return;
	}

	std::istringstream	request_line( line );
	std::string 		method, uri, version;
	request_line >> method >> uri >> version;
	if( method.empty() || uri.empty() || version.empty() ){
		setResponseCode( 400 );
		return;
	}

	if( version != "HTTP/1.1" ){
		setResponseCode( 505 );
		return;
	}

	_method = method;
	_uri = uri;
	_version = version;

	// Parse headers
	while( std::getline( ss, line ) && !line.empty() && line != "\r" ){
		
		if( line[line.length()-1] == '\r' )
			line = line.substr( 0, line.length() - 1 );

		size_t colon = line.find( ':' );
		if( colon != std::string::npos ){
			std::string key = line.substr( 0, colon );
			std::string value = line.substr( colon + 2 );

			std::vector<std::string> values;

			// special handling for Cookie header
			if( key == "Cookie" ){

				std::stringstream	valueStream( value );
				std::string		item;

				while( std::getline( valueStream, item, ';' ) ){

					item.erase( 0, item.find_first_not_of(" ") );
					item.erase( item.find_last_not_of(" ") + 1 );
					values.push_back( item );
				}
			} 
			else{

				// normal comma-separated header handling
				std::stringstream	valueStream( value );
				std::string		item;
				while( std::getline( valueStream, item, ',' ) ){

					item.erase( 0, item.find_first_not_of(" ") );
					item.erase( item.find_last_not_of(" ") + 1 );
					values.push_back( item );
				}
			}
			_headers[key] = values;
		}
	}

	// Check to make sure host is in headers
	if( _headers.find( "Host" ) == _headers.end() ){
		setResponseCode( 400 );
		return;
	}

	// Set URL from host header + URI and set host (IP + port)
	_hostname = _headers["Host"][0];
	_url = _hostname + _uri;

	// Only handle GET, POST, DELETE
	if( _method != "GET" && _method != "POST" && _method != "DELETE" ){
		setResponseCode( 405 );
		return;
	}
}

// check if a string only contains digits 
bool static isOnlyDigits(const std::string& str) {
	std::istringstream iss(str);
	int number;
	char remaining;

	return iss >> number && !(iss >> remaining);
}

void HttpRequest::parseBodyChunked(const std::string& rawRequest, size_t bodyStart) {
	std::string chunkedBody = rawRequest.substr(bodyStart, (rawRequest.size() - bodyStart) + 1);
	std::stringstream ss(chunkedBody);
	std::string line;
	while (std::getline(ss, line)) {
		// check if line in chunked is byte size of next chunk -> continue for now. implement check for correct byte later
		if (isOnlyDigits(line))
			continue;
		else if (line.empty())
			continue;
		// reached end of chunked request
		else if (line.find("0") != std::string::npos)
			break;
		else
			_body.append(line);
		std::stringstream chunkedline;
		//std::cout << "chunkedline: " << line << '\n';
	}
}

void HttpRequest::parseBody(const std::string& rawRequest) {

	// check that at least one of these headers exists (MUST for POST request)
	if (_headers.find("Content-Length") == _headers.end() && _headers.find("Transfer-Encoding") == _headers.end()) {
		setResponseCode(411); // Length Required
		return;
	}

	// find \r\n\r\n that seperates body and header
	size_t bodyStart = rawRequest.find("\r\n\r\n");
	if (bodyStart == std::string::npos) {
		setResponseCode(400);
		return;
	}
	// Skip the \r\n\r\n
	bodyStart += 4;

	// parse chunked requests seperately (check to make sure transfer encoding exists first. above we only checked if either content length or transfer exists)
	if (_headers.find("Transfer-Encoding") != _headers.end() && _headers["Transfer-Encoding"][0] == "chunked") {
		parseBodyChunked(rawRequest, bodyStart);
		return;
	}

	// Get the body using Content-Length
	// TODO: refactor this
	// this only works when content length is used -> which is not always the case
	size_t contentLength = 0;
	std::istringstream(_headers["Content-Length"][0]) >> contentLength;

	if (bodyStart + contentLength <= rawRequest.length()) {
		std::string bodyContent = rawRequest.substr(bodyStart, contentLength);
		setBody(bodyContent);

	} else {
		std::cerr << "Warning: Incomplete body received. Expected " 
			<< contentLength << " bytes, got " 
			<< (rawRequest.length() - bodyStart) << " bytes." << std::endl;
	}

}

void HttpRequest::parse(const std::string& rawRequest) {

	parseHeaders(rawRequest);

	if ( _method == "POST" ) {
		parseBody(rawRequest);
	}

	// FOR DEBUGGING 
	// std::cout << "DEBUG" << *this << '\n';
}

void HttpRequest::handleRequest(const std::string& rawRequest) {
	// parse http request
	parse(rawRequest);

	// extract hostname
	// hostname = remove port from host if present
	size_t colon = _hostname.find(":");
	if (colon != std::string::npos) {
		std::stringstream ss(_hostname.substr(colon + 1));
		ss >> _port;
		//std::cout << "PORT: " << _port << '\n';
		_hostname = _hostname.substr(0, colon);
	}

	// validateRequestPath();
	if (_response_code != 200)
		return ;
}

std::string HttpRequest::getConnectionType() const {
	// Default connection type based on HTTP version
	std::string connectionType = "keep-alive";


	// Check if there is an explicit Connection header
	std::map<std::string, std::vector<std::string> >::const_iterator it = _headers.find("Connection");
	if (it != _headers.end() && !it->second.empty()) {
		std::string clientConnection = it->second[0];

		// Convert to lowercase for case-insensitive comparison
		std::string lowerConnection = clientConnection;
		std::transform(lowerConnection.begin(), lowerConnection.end(), 
				lowerConnection.begin(), ::tolower);

		// Check if it's either keep-alive or close
		if (lowerConnection == "keep-alive" || lowerConnection == "close") {
			connectionType = lowerConnection;
		}
	}

	return connectionType;
}
