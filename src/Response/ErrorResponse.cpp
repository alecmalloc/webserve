#include "webserv.hpp"

ErrorResponse::ErrorResponse():
_isServerConf(false)
{

}

ErrorResponse::ErrorResponse(const int& statusCode, const HttpRequest& request , const std::vector<ServerConf>& serverConfs): 
_statusCode(statusCode),
_serverConfs(serverConfs), 
_request(request),
_isServerConf(false)
{
    matchServerBlock();
}

ErrorResponse::ErrorResponse(const ErrorResponse& other) {
    _serverConfs = other._serverConfs;
    _serverConf = other._serverConf;
    _request = other._request;
    _isServerConf = other._isServerConf;
    _httpResponse = other._httpResponse;
    _headers = other._headers;
    _serverName = other._serverName;
}

ErrorResponse::~ErrorResponse() {}

const ErrorResponse& ErrorResponse::operator =(const ErrorResponse& other) {
    if (this != &other) {
        _serverConfs = other._serverConfs;
        _serverConf = other._serverConf;
        _request = other._request;
        _isServerConf = other._isServerConf;
        _httpResponse = other._httpResponse;
        _headers = other._headers;
        _serverName = other._serverName;
    }
    return *this;
}

void ErrorResponse::matchServerName( void ) {

	std::string hostname = _request.getHostname();

	std::vector<std::string> serverNames = _serverConf.getServerConfNames();
	// if we can find req hostname match with string in serverNames
 	std::vector<std::string>::iterator it =  std::find(serverNames.begin(), serverNames.end(), hostname);

	_serverName = ( it != serverNames.end() ) ?  hostname : "Webserv";
}

void ErrorResponse::matchServerBlock() {
	// match server block based on port
	for (size_t i = 0; i < _serverConfs.size(); i++) {
		// server block we are currently inspecting

		// get ip ports map
		const std::map<std::string, std::set<int> > ipsPorts =  _serverConfs[i].getIpPort();
		// loop through map
		for (std::map<std::string, std::set<int> >::const_iterator it = ipsPorts.begin(); it != ipsPorts.end(); ++it) {
			// check if we can find port in set
			if (it->second.find(_request.getPort()) != it->second.end()) {
				_serverConf = _serverConfs[i];
                _isServerConf = true;
                break;
			}
		}
	}
    
	// default isserverconf is false so no need to set
}

void ErrorResponse::matchErrorPage() {
    // load error pages
    const std::map<int, std::string>& errorPages = _serverConf.getErrorPages();

    // try to find
    std::map<int, std::string>::const_iterator it = errorPages.find(_statusCode);

    // case found error page
    if (it != errorPages.end()) {
        _serverConf.getRootDir() + 
    }
}

static std::string intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

static std::string generateGenericErrorPage() {
    std::string html;



    return html;
}

static std::string getCurrentDateTime() {
    std::time_t now = std::time(0);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}


static std::string generateReasonPhrase(int statusCode) {
    std::string reasonPhrase;

    // matches http code to a string for response
    switch (statusCode){
            case 100:
            reasonPhrase = "Continue";
            break;
        case 101:
            reasonPhrase = "Switching Protocols";
            break;

        // 2xx: Success
        case 200:
            reasonPhrase = "OK";
            break;
        case 201:
            reasonPhrase = "Created";
            break;
        case 202:
            reasonPhrase = "Accepted";
            break;
        case 204:
            reasonPhrase = "No Content";
            break;

        // 3xx: Redirection
        case 301:
            reasonPhrase = "Moved Permanently";
            break;
        case 302:
            reasonPhrase = "Found";
            break;
        case 304:
            reasonPhrase = "Not Modified";
            break;

        // 4xx: Client Error
        case 400:
            reasonPhrase = "Bad Request";
            break;
        case 401:
            reasonPhrase = "Unauthorized";
            break;
        case 403:
            reasonPhrase = "Forbidden";
            break;
        case 404:
            reasonPhrase = "Not Found";
            break;
        case 405:
            reasonPhrase = "Method Not Allowed";
            break;
        case 413:
            reasonPhrase = "413 Request Entity Too Large";
            break;
            // 5xx: Server Error
        case 500:
            reasonPhrase = "Internal Server Error";
            break;
        case 501:
            reasonPhrase = "Not Implemented";
            break;
        case 502:
            reasonPhrase = "Bad Gateway";
            break;
        case 503:
            reasonPhrase = "Service Unavailable";
            break;
        case 505:
            reasonPhrase = "HTTP Version Not Supported";
            break;

        // Default case for unknown status codes
        default:
            reasonPhrase = "Error";
            break;
    }
}

static std::string generateStatusLine(int statusCode) {
    std::string statusLine;
    statusLine += HTTP_VERSION;
    statusLine += " ";
    statusLine +=  intToString(statusCode);
    statusLine += " ";
    statusLine += generateReasonPhrase(statusCode);
    return statusLine;
}

void ErrorResponse::generateHeaders() {
    std::string headersStr;

    headersStr += generateStatusLine(_statusCode) + "\r\n";
    headersStr += "Date: " + getCurrentDateTime() + "\r\n";
    headersStr += "Server: " + _serverName + "\r\n";
    headersStr += "Content-Type: text/html\r\n";
    headersStr += "Content-Length: " + intToString(getBody().length()) + "\r\n";
    headersStr += "Connection: " + _request.getConnectionType() + "\r\n";
    headerStr += "\r\n"; // End of headers

    _headers = headerStr;
    // setHttpResponse(responseString);
}

void ErrorResponse::generateHttpResponse() {
    std::string head;
    std::string body;

    // check errorPage.empty()
    // handle customerrorpage
    // else handle generic page

    // do this at end because i need to know body length
    head += generateHeaders();

    _httpResponse = head + body;
}