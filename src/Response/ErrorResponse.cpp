#include "webserv.hpp"

ErrorResponse::ErrorResponse():
_isServerConf(false)
{
	;
}

ErrorResponse::ErrorResponse(const int& statusCode, const HttpRequest& request , const std::vector<ServerConf>& serverConfs): 
_statusCode(statusCode),
_serverConfs(serverConfs),
_request(request),
_isServerConf(false)
{
    matchServerBlock();

    // only check for error page if we find a server conf
    // only check server name if we find server conf
    if (_isServerConf) {
        matchErrorPage();
        _serverName = matchServerName(_request, _serverConf);
    }
}

ErrorResponse::ErrorResponse(const ErrorResponse& other) {
    _serverConfs = other._serverConfs;
    _serverConf = other._serverConf;
    _request = other._request;
    _isServerConf = other._isServerConf;
    _httpResponse = other._httpResponse;
    _headers = other._headers;
    _serverName = other._serverName;
    _errorPage = other._errorPage;
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
        _errorPage = other._errorPage;
    }
    return *this;
}

std::string ErrorResponse::getHttpResponse( void ) {
    return _httpResponse;
}

void ErrorResponse::matchServerBlock() {
	// match server block based on port
	for (size_t i = 0; i < _serverConfs.size(); i++) {
		// server block we are currently inspecting

		// get ip ports map
		const std::map<std::string, std::set<int> > ipsPorts =  _serverConfs[i].getIpPort();
		// loop through map
		for (std::map<std::string, std::set<int> >::const_iterator it = ipsPorts.begin(); it != ipsPorts.end(); it++) {
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
    // maybe double slash issue? i think cpp handles this tho ie: .//errorpage.html
    if (it != errorPages.end()) {
        _errorPage = _serverConf.getRootDir() + it->second;
    }
    // otherwise errorPage stays empty and its a str so we can check that
}

static std::string generateGenericErrorPage(int statusCode) {
    std::string html;

    html += "<html><body>";
    html += "<h1>Error: ";
    html += ::intToString(statusCode);
    html += "</h1>";
    html += "<p>" + generateReasonPhrase(statusCode) + "</p>";
    html += "</body></html>";
    html += CSS_GLOBAL;

    return html;
}

std::string ErrorResponse::generateHeaders(std::string& body) {
    std::string headerStr;

    headerStr += ::generateStatusLine(_statusCode) + "\r\n";
    headerStr += "Date: " + getCurrentDateTime() + "\r\n";
    headerStr += "Server: " + _serverName + "\r\n";
    headerStr += "Content-Type: text/html\r\n";
    headerStr += "Content-Length: " + ::intToString(body.length()) + "\r\n";
    headerStr += "Connection: " + _request.getConnectionType() + "\r\n";
    headerStr += "\r\n"; // End of headers

    return headerStr;
}

void ErrorResponse::generateHttpResponse() {
    std::string head;
    std::string body;

    // BODY
    // handle having an error page
    if ( !(_errorPage.empty()) ) {

        std::ifstream file(_errorPage.c_str());

        if (file) {
			std::ostringstream contents;
			contents << file.rdbuf();
			file.close();
			body = contents.str();
        }
        else
            _statusCode = 500;
    }

    // handle file not being able to open (see else above) or no custom error pages 
    if (body.empty())
        body = generateGenericErrorPage(_statusCode);

    // HEAD
    // do this at end because i need to know body length
    // pass body to calc length
    head = generateHeaders(body);

    _httpResponse = head + body;
}
