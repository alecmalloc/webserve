#include "webserv.hpp"

std::string matchServerName( HttpRequest& request, ServerConf& serverConf ) {

	std::string hostname = request.getHostname();

	std::vector<std::string> serverNames = serverConf.getServerConfNames();
	// if we can find req hostname match with string in serverNames
 	std::vector<std::string>::iterator it =  std::find(serverNames.begin(), serverNames.end(), hostname);

	std::string serverName = ( it != serverNames.end() ) ?  hostname : "Webserv";
    return serverName;
}

std::string generateStatusLine(int statusCode) {
    std::string statusLine;
    statusLine += HTTP_VERSION;
    statusLine += " ";
    statusLine +=  ::intToString(statusCode);
    statusLine += " ";
    statusLine += ::generateReasonPhrase(statusCode);
    return statusLine;
}

std::string getCurrentDateTime() {
    std::time_t now = std::time(0);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}

std::string getContentType( HttpRequest& request, PathInfo& pathInfo ) {
    // POST AND DELETE always return html
    if (request.getMethod() == "POST" || request.getMethod() == "DELETE")
        return("text/html");

    // FOR GET
    static const std::pair<std::string, std::string> contentTypes[] = {
        std::pair<std::string, std::string>("html", "text/html"),
        std::pair<std::string, std::string>("htm", "text/html"),
        std::pair<std::string, std::string>("css", "text/css"),
        std::pair<std::string, std::string>("js", "application/javascript"),
        std::pair<std::string, std::string>("json", "application/json"),
        std::pair<std::string, std::string>("png", "image/png"),
        std::pair<std::string, std::string>("jpg", "image/jpeg"),
        std::pair<std::string, std::string>("jpeg", "image/jpeg"),
        std::pair<std::string, std::string>("gif", "image/gif"),
        std::pair<std::string, std::string>("svg", "image/svg+xml"),
        std::pair<std::string, std::string>("ico", "image/x-icon"),
        std::pair<std::string, std::string>("txt", "text/plain")
        // Add more content types as needed
    };

    static const size_t contentTypesCount = sizeof(contentTypes) / sizeof(contentTypes[0]);

    for (size_t i = 0; i < contentTypesCount; ++i) {
        if (contentTypes[i].first == pathInfo.getExtension()) {
            return contentTypes[i].second;
        }
    }

    // default cause -> why pdf files for instance werent working
	return("text/html");
}

std::string generateDirectoryListing(const std::string& path) {
    std::stringstream html;
    DIR *dir;
    struct dirent *entry;


    if ((dir = opendir(path.c_str())) == NULL)
	    throw( 404 );

    html << "<html>\n<head>\n"
	    << "<title>Index of " << path << "</title>\n"
	    << "<style>table { width: 100%; } th { text-align: left; }</style>\n"
	    << "</head>\n<body>\n"
	    << "<h1>Index of " << path << "</h1>\n"
	    << "<table>\n"
	    << "<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";
    while ((entry = readdir(dir)) != NULL) {
	    std::string name = entry->d_name;

	    // Skip "." and ".."
	    if (name == "." || name == "..") {
		    continue;
	    }

	    std::string fullPath = path + "/" + name;
	    struct stat statbuf;

	    if (stat(fullPath.c_str(), &statbuf) == 0) {
		    // Format last modified time
		    char timeStr[100];
		    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S",
				    localtime(&statbuf.st_mtime));
		    // Add row to table
		    html << "<tr><td><a href=\"";
		    if (S_ISDIR(statbuf.st_mode)) {
			    html << name << "/";
			    html << "\">" << name << "/";
		    }
		    else{
			    html << name; // Append only the file/directory name
			    html << "\">" << name;
		    }
		    html << "</a></td><td>" << timeStr << "</td><td>";
		    if (!S_ISDIR(statbuf.st_mode)) {
			    html << statbuf.st_size;
		    }
		    html << "</td></tr>\n";
	    }
    }
    closedir(dir);
    html << "</table>\n</body>\n</html>";

    return html.str();
}

std::string intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string generateReasonPhrase(int httpCode) {

	std::string reasonPhrase;

    // matches http code to a string for response
	switch (httpCode){
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
        case 504:
            reasonPhrase = "Gateway Timeout";
            break;
        case 505:
            reasonPhrase = "HTTP Version Not Supported";
            break;

        // Default case for unknown status codes
        default:
            reasonPhrase = "We aren't sure why this failed ...";
            break;
    }

	return(reasonPhrase);
}
