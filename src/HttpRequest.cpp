#include "webserv.hpp"

HttpRequest::HttpRequest(Config& conf): 
    _conf(conf), 
    _response_code(200), 
    _pathInfo() 
{
    ;
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

void HttpRequest::parse(const std::string& rawRequest) {

    // split request data into lines
    std::stringstream ss(rawRequest);
    std::string line;
    // make sure first line exists again (just to be sure)
    if (!std::getline(ss, line)) {
        setResponseCode(400);
        return;
    }
    // parse request line (METHOD URI HTTP/VERSION)
    std::istringstream request_line(line);
    std::string method, uri, version;
    request_line >> method >> uri >> version;
    // check if method uri and version exist
    if (method.empty() || uri.empty() || version.empty()) {
        setResponseCode(400);
        return;
    }
    
    // check if http version is 1.1 (correct version)
    if (version != "HTTP/1.1") {
        setResponseCode(505);
        return;
    }

    // set ss to member variables
    _method = method;
    _uri = uri;
    _version = version;

    while (std::getline(ss, line) && !line.empty() && line != "\r") {
        if (line[line.length()-1] == '\r')
            line = line.substr(0, line.length()-1);
    
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
                    // trim leading/trailing whitespace
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

    // check to make sure host is in headers -> these are standard and sent with every req in modern tools, however just to be safe
    if (_headers.find("Host") == _headers.end()) {
        setResponseCode(400);
        return;
    }

    // set url from host header + uri and set host (ip + port)
    std::string host = _headers["Host"][0];
    _url = host + _uri;

    // only handle GET POST DELETE
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        setResponseCode(405);
        return;
    }

    // POST specific validation
    if (_method == "POST") {
        // check for Content-Length or Transfer-Encoding header
        if (_headers.find("Content-Length") == _headers.end() && _headers.find("Transfer-Encoding") == _headers.end()) {
            setResponseCode(411); // Length Required
            return;
        }

        // verify body exists for POST
        if (_body.empty()) {
            setResponseCode(400); // Bad Request
            return;
        }
    }

    // get body if present (everything after empty line)
    std::string body;
    while (std::getline(ss, line)) {
        body += line + "\n";
    }
    if (!body.empty()) {
        _body = body;
    }

    // //  match server block from conf
    std::vector<ServerConf> server_list;
    server_list = _conf.getServerConfs();
    // hostname = remove port from host if present
    size_t colon = host.find(":");
    std::string hostname;
    if (colon != std::string::npos)
        _hostname = host.substr(0, colon);
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
    // return if above fails and return response? -> cant remember what i meant with this lol
}

void HttpRequest::handleRequest(const std::string& rawRequest) {
    // parse http request
    parse(rawRequest);
    if (_response_code != 200)
        return ;

    // validate and load into PathInfo obj
    validateRequestPath();
    if (_response_code != 200)
        return ;
}

void HttpRequest::validateRequestPath(void) {
    // grab location confs

    // things we need in for loop
    const std::vector<LocationConf>& locationConfs = _server.getLocationConfs();
    std::string bestMatch = "";
    const LocationConf* matchedLoc = NULL;
    const std::string uri = getUri();

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
    
    // todo set path info to full path
    std::string fullPath = _server.getRootDir() + bestMatch;
    std::cout << fullPath  << '\n';

    if ((_response_code = _pathInfo.validatePath() != 200))
        return;
}