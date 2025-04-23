#include "webserv.hpp"
#include <ctime>



void	Response::generateStatusLine(HttpRequest &reqObj){
	std::string httpVersion = reqObj.getVersion();
	std::string statusCode = Response::intToString(reqObj.getResponseCode());
	std::string reasonPhrase = genarateReasonPhrase(reqObj.getResponseCode());
	std::stringstream statusLine;
    statusLine << httpVersion << " " << statusCode << " " << reasonPhrase;
    setStatusLine(statusLine.str());
}

std::string Response::getCurrentDateTime() {
    std::time_t now = std::time(0);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}


std::string Response::getContentType(HttpRequest &reqObj, const std::string& extension) {
	//std::cout << "extetion:" + extension +":\n";
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
        if (contentTypes[i].first == extension) {
            return contentTypes[i].second;
        }
    }
	if(reqObj.getResponseCode() != 200)
		return "text/html; charset=utf-8"; // might not be correct but if error is html response ?
	return("text/html");//change so default is html
		//return "application/octet-stream"; // Default content type
}


//need to adjust to use right values
void	Response::generateHeader(HttpRequest &reqObj){
	generateStatusLine(reqObj);

	std::map<std::string, std::string> headerMap;
	headerMap["Status-Line"] = getStatusLine();
    headerMap["Date"] = getCurrentDateTime();
	if (_serverConf)
        headerMap["Server"] = getServerName();
    
	// Get the file extension from the PathInfo object

    std::string extension = reqObj.getPathInfo().getExtension();
    headerMap["Content-Type"] = getContentType(reqObj, extension);

	headerMap["Content-Length"] =  intToString(getBody().length());
	headerMap["Connection"] = reqObj.getConnectionType();

    // only if setCookieValue has been changed
    if (_setCookieValue != "") {
        headerMap["Set-Cookie"] = _setCookieValue;
    }

	// Optional Fields
   // headerMap["Cache-Control"] = "no-cache"; // Example value
    //headerMap["Set-Cookie"] = "sessionId=abc123; Path=/; HttpOnly"; // Example value
    //headerMap["Last-Modified"] = getCurrentDateTime(); // Example value
    //headerMap["ETag"] = "\"123456789\""; // Example value
    //headerMap["Location"] = "/new-resource"; // Example value

    // for redirects
    if (reqObj.getResponseCode() == 301 || reqObj.getResponseCode() == 302) {
        headerMap["Location"] = getRedirectDest();
    }

	if(reqObj.getResponseCode() == 201){//for Post methode specify headers
		headerMap["Location"] = reqObj.getUri(); // Stay on same page
		headerMap["Content-Type"] = "text/html; charset=utf-8"; // Specify charset
		headerMap["Cache-Control"] = "no-store, no-cache, must-revalidate";
		headerMap["Pragma"] = "no-cache";
	}

	setHeaderMap(headerMap);
}

std::string Response::generateDirectoryListing(const std::string& path) {
    std::stringstream html;
    DIR *dir;
    struct dirent *entry;

    html << "<html>\n<head>\n"
         << "<title>Index of " << path << "</title>\n"
         << "<style>table { width: 100%; } th { text-align: left; }</style>\n"
         << "</head>\n<body>\n"
         << "<h1>Index of " << path << "</h1>\n"
         << "<table>\n"
         << "<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

    if ((dir = opendir(path.c_str())) != NULL) {
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
                html << name; // Append only the file/directory name
                if (S_ISDIR(statbuf.st_mode)) {
                    html << "/"; // Add trailing slash for directories
                }
                html << "\">" << name;
                if (S_ISDIR(statbuf.st_mode)) {
                    html << "/";
                }
                html << "</a></td><td>" << timeStr << "</td><td>";
                if (!S_ISDIR(statbuf.st_mode)) {
                    html << statbuf.st_size;
                }
                html << "</td></tr>\n";
            }
        }
        closedir(dir);
    }

    html << "</table>\n</body>\n</html>";
    return html.str();
}


std::string Response::intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string Response::genarateReasonPhrase(int httpCode){

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
        case 505:
            reasonPhrase = "HTTP Version Not Supported";
            break;

        // Default case for unknown status codes
        default:
            reasonPhrase = "Error";
            break;
    }

    // sets response phrase in obj and also returns it
	setReasonPhrase(reasonPhrase);
	return(reasonPhrase);
}
