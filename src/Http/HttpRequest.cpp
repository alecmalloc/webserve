#include "webserv.hpp"

HttpRequest::HttpRequest(Config& conf):
    _conf(conf),
    _response_code(200),
    _port(-1),
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

int HttpRequest::getPort() const {
    return _port;
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

void	HttpRequest::setPathInfoPath( std::string path ){
	_pathInfo.setFullPath( path );
}

std::map<std::string, std::vector<std::string> > HttpRequest::getHeaders() const {
    return _headers;
}

std::string HttpRequest::getBody() const {
    return _body;
}

void HttpRequest::setPort(int port) {
    _port = port;
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
        
    } else {
        std::cerr << "Warning: Incomplete body received. Expected " 
                    << contentLength << " bytes, got " 
                    << (rawRequest.length() - bodyStart) << " bytes." << std::endl;
    }

}

// //  match server block from conf
// this doesnt even work lol, only checks for hostname in IPS??
void HttpRequest::matchServerBlock(void) {
    std::vector<ServerConf> serverTmpConf = _conf.getServerConfs();

    // Match server based on hostname and port
    for (size_t i = 0; i < serverTmpConf.size(); i++) {
        // Check server names
        const std::vector<std::string>& serverNames = serverTmpConf[i].getServerConfNames();
        if (std::find(serverNames.begin(), serverNames.end(), _hostname) != serverNames.end()) {
            // Check if port matches too
            const std::map<std::string, std::set<int> > ports = serverTmpConf[i].getIpPort();
            for (std::map<std::string, std::set<int> >::const_iterator it = ports.begin(); it != ports.end(); ++it) {
                if (it->second.find(_port) != it->second.end()) {
                    _server = serverTmpConf[i];
                    return;
                }
            }
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
    if (colon != std::string::npos) {
        std::stringstream ss(_hostname.substr(colon + 1));
        ss >> _port;
        std::cout << "PORT: " << _port << '\n';
        _hostname = _hostname.substr(0, colon);
    }

    // match server block from conf
    matchServerBlock();

    validateRequestPath();
    if (_response_code != 200)
        return ;
}

void HttpRequest::validateRequestPath(void) {
    const std::vector<LocationConf>& locationConfs = _server.getLocationConfs();
    const std::string uri = getUri();

    // Protect against empty/null values
    if (_server.getRootDir().empty()) {
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
                    std::cout << "Warning: Location has no index files" << '\n';
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
    _pathInfo.validatePath();
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
