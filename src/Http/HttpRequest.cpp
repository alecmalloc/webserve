#include "webserv.hpp"

HttpRequest::HttpRequest(Config& conf):
    _conf(conf),
    _response_code(200),
    _pathInfo()
{
    _cgiResponseString = "";
}

HttpRequest::HttpRequest(const HttpRequest& other):
    _conf(other._conf),
    _response_code(other._response_code),
    _method(other._method),
    _uri(other._uri),
    _url(other._url),
    _version(other._version),
    _headers(other._headers),
    _body(other._body),
    _hostname(other._hostname),
    _server(other._server),
    _pathInfo(other._pathInfo)
{
    ;
}

HttpRequest& HttpRequest::operator =(const HttpRequest& other) {
    if (this != &other) {
        _conf = other.getConf();
        _method = other.getMethod();
        _uri = other.getUri();
        _url = other.getUrl();
        _version = other.getVersion();
        _headers = other.getHeaders();
        _body = other.getBody();
        _hostname = other.getHostname();
        _response_code = other.getResponseCode();
        _server = other.getServer();
        _pathInfo = other.getPathInfo();
    }

    return (*this);
}


std::string HttpRequest::getCgiResponseString() const {
    return _cgiResponseString;
}


ServerConf HttpRequest::getServer() const {
    return _server;
}

std::string HttpRequest::getHostname() const {
    return _hostname;
}

Config& HttpRequest::getConf() const {
    return _conf;
}

int HttpRequest::getResponseCode() const {
    return _response_code;
}

std::string HttpRequest::getUri() const {
    return _uri;
}

std::string HttpRequest::getUrl() const {
    return _url;
}

std::string HttpRequest::getVersion() const {
    return _version;
}

std::string HttpRequest::getMethod() const {
    return _method;
}

PathInfo HttpRequest::getPathInfo() const {
    return _pathInfo;
}


void HttpRequest::setCgiResponseString(const std::string& cgiResponseString) {
    _cgiResponseString = cgiResponseString;
}


void	HttpRequest::setMethod( std::string tmp ){
	_method = tmp;
}

void	HttpRequest::setUri( std::string tmp ){
	_uri = tmp;
}

void	HttpRequest::setUrl( std::string tmp ){
	_url = tmp;
}

void	HttpRequest::setVersion( std::string tmp ){
	_version = tmp;

}

void	HttpRequest::setBody( std::string tmp ){
	_body = tmp;
}

void	HttpRequest::setHeader( std::string tmp1, std::string tmp2 ){
	_headers[ tmp1 ].push_back( tmp2 );
}

void	HttpRequest::setConfig( Config& tmp ){
	_conf = tmp;
}

void	HttpRequest::setResponseCode( int tmp ){
	_response_code = tmp;
}

void	HttpRequest::setServer( ServerConf tmp ){
	_server = tmp;
}

void    HttpRequest::setPathInfo( PathInfo tmp ) {
    _pathInfo = tmp;
}

std::map<std::string, std::vector<std::string> > HttpRequest::getHeaders() const {
    return _headers;
}

std::string HttpRequest::getBody() const {
    return _body;
}

// overload for printing
std::ostream& operator<<(std::ostream& os, HttpRequest& request) {
    os << "Version: " << request.getVersion() << "\n";
    os << "Method: " << request.getMethod() << "\n";
    os << "Uri: " << request.getUri() << "\n";
    os << "Url: " << request.getUrl() << "\n";
    os << "Headers: \n";
    const std::map<std::string, std::vector<std::string> >& headers = request.getHeaders();
    for (std::map<std::string, std::vector<std::string> >::const_iterator header = headers.begin(); header != headers.end(); ++header) {
        os << "  " << header->first << ": ";
        for (std::vector<std::string>::const_iterator value = header->second.begin(); value != header->second.end(); ++value) {
            os << *value << " ";
        }
        os << "\n";
    }
    os << "Body: " << request.getBody() << "\n";
    return os;
}

void HttpRequest::parseHeaders(const std::string& rawRequest) {
    // split request data into lines
    std::stringstream ss(rawRequest);
    std::string line;

    // Parse request line (METHOD URI HTTP/VERSION)
    if (!std::getline(ss, line)) {
        setResponseCode(400);
        return;
    }
    std::istringstream request_line(line);
    std::string method, uri, version;
    request_line >> method >> uri >> version;
    if (method.empty() || uri.empty() || version.empty()) {
        setResponseCode(400);
        return;
    }
    if (version != "HTTP/1.1") {
        setResponseCode(505);
        return;
    }
    _method = method;
    _uri = uri;
    _version = version;

    // Parse headers
    while (std::getline(ss, line) && !line.empty() && line != "\r") {
        if (line[line.length()-1] == '\r')
            line = line.substr(0, line.length() - 1);

        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 2); // skip ": "

            std::vector<std::string> values;

            // special handling for Cookie header
            if (key == "Cookie") {
                std::stringstream valueStream(value);
                std::string item;
                while (std::getline(valueStream, item, ';')) {
                    item.erase(0, item.find_first_not_of(" "));
                    item.erase(item.find_last_not_of(" ") + 1);
                    values.push_back(item);
                }
            } else {
                // normal comma-separated header handling
                std::stringstream valueStream(value);
                std::string item;
                while (std::getline(valueStream, item, ',')) {
                    item.erase(0, item.find_first_not_of(" "));
                    item.erase(item.find_last_not_of(" ") + 1);
                    values.push_back(item);
                }
            }

            _headers[key] = values;
        }
    }

    // Check to make sure host is in headers
    if (_headers.find("Host") == _headers.end()) {
        setResponseCode(400);
        return;
    }

    // Set URL from host header + URI and set host (IP + port)
    _hostname = _headers["Host"][0];
    _url = _hostname + _uri;

    // Only handle GET, POST, DELETE
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        setResponseCode(405);
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

        // Debug output
        // std::cout << "Read POST body, length: " << bodyContent.length() << std::endl;
    } else {
        std::cerr << "Warning: Incomplete body received. Expected " 
                    << contentLength << " bytes, got " 
                    << (rawRequest.length() - bodyStart) << " bytes." << std::endl;
    }

}

// //  match server block from conf
void HttpRequest::matchServerBlock(void) {
    std::vector<ServerConf> server_list;
    server_list = _conf.getServerConfs();

    // loop over serverConfs and match server names and ips
    for (std::vector<ServerConf>::iterator it = server_list.begin(); it != server_list.end(); ++it) {
        std::vector<std::string> server_names = it->getServerConfNames();
        std::map<std::string, std::set<int> > ipPorts = it->getIpPort();

        if (std::find(server_names.begin(), server_names.end(), _hostname) != server_names.end()) {
            _server = (*it);
        }
        if (ipPorts.find(_hostname) != ipPorts.end()) {
            _server = (*it);
        }
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
    if (colon != std::string::npos)
        _hostname = _hostname.substr(0, colon);

    // match server block from conf
    matchServerBlock();

	//std::cout << "raw req string :" << rawRequest << ":\n";
	// validate and load into PathInfo obj
    validateRequestPath();
    if (_response_code != 200)
        return ;
}

void HttpRequest::validateRequestPath(void) {
    const std::vector<LocationConf>& locationConfs = _server.getLocationConfs();
    const std::string uri = getUri();
    
    // Add null checks and debugging
    //std::cout << "Processing URI: " << uri << std::endl;
    //std::cout << "Server root: " << _server.getRootDir() << std::endl;

    // Protect against empty/null values
    if (_server.getRootDir().empty()) {
        // std::cout << "Warning: Server root is empty" << std::endl;
        _response_code = 500;
        return;
    }

    // Create initial PathInfo with safety checks
    std::string fullPath = _server.getRootDir();
    if (!uri.empty()) {
        fullPath += uri;
    }
    _pathInfo = PathInfo(fullPath);
    
    for (std::vector<LocationConf>::const_iterator it = locationConfs.begin();
         it != locationConfs.end(); ++it) {
        // Add null checks before accessing location properties
        if (it->getPath().empty()) {
            continue;
        }

        if (uri.find(it->getPath()) == 0) {
            if (uri == it->getPath() || uri == it->getPath() + "/") {
                // Check if index array is not empty before accessing first element
                if (!it->getIndex().empty()) {
                    std::string locationPath = it->getRootDir();
                    locationPath += it->getPath();
                    locationPath += "/";
                    locationPath += it->getIndex()[0];
                    _pathInfo = PathInfo(locationPath);
                } else {
                    std::cout << "Warning: Location has no index files" << std::endl;
                    _response_code = 404;
                    return;
                }
            } else {
                std::string locationPath = it->getRootDir() + uri;
                _pathInfo = PathInfo(locationPath);
            }
            break;
        }
    }

    _pathInfo.parsePath();

    if ((_response_code = _pathInfo.validatePath()) != 200)
        return;
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


/*
void HttpRequest::validateRequestPath(void) {
    const std::vector<LocationConf>& locationConfs = _server.getLocationConfs();
    std::string bestMatch = "";
    const LocationConf* matchedLoc = NULL;
    const std::string uri = getUri();

    // Set PathInfo before location matching
    std::string full_path = _server.getRootDir() + uri;
    _pathInfo = PathInfo(full_path);
    _pathInfo.parsePath(); // Parse the path components regardless of validation

    // Loop over location confs
    for (std::vector<LocationConf>::const_iterator it = locationConfs.begin();
        it != locationConfs.end(); ++it) {
            const LocationConf& loc = *it;
            std::string locPath = loc.getPath();
            if (uri.substr(0, locPath.length()) == locPath) {
                if (locPath.length() > bestMatch.length()) {
                    bestMatch = locPath;
                    matchedLoc = &loc;
                }
            }
    }

    // If no location match, set 404 but keep path info
    if (!matchedLoc) {
        _response_code = 404;
        return;
    }

    // Only validate the path exists if we found a matching location
    if ((_response_code = _pathInfo.validatePath()) != 200)
        return;
}



void HttpRequest::validateRequestPath(void) {
    // grab location confs

    // things we need in for loop
    const std::vector<LocationConf>& locationConfs = _server.getLocationConfs();
    std::string bestMatch = "";
    const LocationConf* matchedLoc = NULL;
    const std::string uri = getUri();

    //std::cout << "Debug: URI to match: " << uri << std::endl;
    //std::cout << "Debug: Number of location configs: " << locationConfs.size() << std::endl;

    // loop over location confs
    for (std::vector<LocationConf>::const_iterator it = locationConfs.begin();
        it != locationConfs.end(); ++it) {
            const LocationConf& loc = *it;
            std::string locPath = loc.getPath();
            // Check if URI starts with location path
            if (uri.substr(0, locPath.length()) == locPath) {
                // keep longest match (most specific). Ex: /posts/ or posts/articles
                if (locPath.length() > bestMatch.length()) {
                    bestMatch = locPath;
                    matchedLoc = &loc;
                }
            }
    }
    // if we couldnt match a location from the locationConfs in the server
    if (!matchedLoc) {
        _response_code = 404;
        return;
    }

    // oarse full path from reuqest uri
    std::string full_path = _server.getRootDir() + uri;
    size_t question_mark = full_path.find("?");
    if (question_mark != std::string::npos)
        full_path = full_path.substr(0, question_mark);

    // std::cout << "Debug fullPath:" << full_path  << '\n';
    // load full path into PathInfo obj
    _pathInfo = PathInfo(full_path);

    // validates path and set reponse code to return val
    if ((_response_code = _pathInfo.validatePath()) != 200)
        return;
    // parse path and set reponse code to return val
    if ((_response_code = _pathInfo.parsePath()) != 200)
        return;
}
*/