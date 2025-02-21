//#include "../inc/Response.hpp"
#include "webserv.hpp"
#include <sstream>
#include <ctime>


std::string getCurrentDateTime() {
    std::time_t now = std::time(0);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}

Response::~Response(){}

Response::Response()
    : _serverConf(NULL), _locationConf(NULL){
    // Default constructor implementation
}

void Response::setReasonPhrase(const std::string &reasonPhrase) {
    _reasonPhrase = reasonPhrase;
}

std::string Response::getReasonPhrase() const {
    return _reasonPhrase;
}
Response::Response(HttpRequest& reqObj,ServerConf* serverConf)
    : _serverConf(serverConf), _locationConf(NULL){
	// Get locations from server config
    std::string uri = reqObj.getUri();

	const std::vector<LocationConf>& locations = serverConf->getLocationConfs();

    // Find best matching location (longest prefix match)
    std::string bestMatch = "";
    
    for (std::vector<LocationConf>::const_iterator it = locations.begin();
         it != locations.end(); ++it) {
        std::string locPath = it->getPath();
        
        if (uri.find(locPath) == 0) {  // URI starts with location path
            if (locPath.length() > bestMatch.length()) {
                bestMatch = locPath;
                _locationConf = const_cast<LocationConf*>(&(*it));
            }
        }
    }
	if (_locationConf){
		std::cout << "LOCATION CONF " << *_locationConf ;
	}
	
		//try{
			processResponse(reqObj);
		//}
	
		//catch(int errror)
		//{
		//	std::cerr << "Caught error: "<< std::endl;
		//	generateErrorResponse(reqObj);
			//make error header look for custom errror sites if none genarate one
		//}
		generateHttpresponse(reqObj);
	}



void Response::generateHttpresponse(HttpRequest &reqObj){
	generateHeader(reqObj);
	std::map<std::string, std::string> headerMap = getHeaderMap();
	std::stringstream header ;
    header << headerMap["Status-Line"] << "\r\n";
    header << "Date: " << headerMap["Date"] << "\r\n";
    header << "Server: " << headerMap["Server"] << "\r\n";
    header << "Content-Type: " << headerMap["Content-Type"] << "\r\n";
    header << "Content-Length: " << headerMap["Content-Length"] << "\r\n";
    header << "Connection: " << headerMap["Connection"] << "\r\n";
    header << "Cache-Control: " << headerMap["Cache-Control"] << "\r\n";
    header << "Set-Cookie: " << headerMap["Set-Cookie"] << "\r\n";
    header << "Last-Modified: " << headerMap["Last-Modified"] << "\r\n";
    header << "ETag: " << headerMap["ETag"] << "\r\n";
    //header << "Location: " << headerMap["Location"] << "\r\n";
    header << "\r\n"; // End of headers

	std::string responseString = header.str() + getBody();
	setHttpResponse(responseString);
}





void	Response::generateStatusLine(HttpRequest &reqObj){
	std::string httpVersion = reqObj.getVersion();
	std::string statusCode = Response::intToString(reqObj.getResponseCode());
	std::string reasonPhrase = genarateReasonPhrase(reqObj);

	std::stringstream statusLine;
    statusLine << httpVersion << " " << statusCode << " " << reasonPhrase;
    setStatusLine(statusLine.str());


}

std::string Response::getServerName(){
	std::vector<std::string> server = _serverConf->getServerConfNames();
	
	// Check if the vector has any elements
    if (!server.empty()) {
        // Return the first server name from the vector
        return server[0];
    }
	//else default value and return
	return("Webserv/1.0");
}


std::string getContentType(const std::string& extension) {
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
    return "application/octet-stream"; // Default content type
}

std::string generateDirectoryListing(const std::string& path) {
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
            std::string fullPath = path + "/" + name;
            struct stat statbuf;
            
            if (stat(fullPath.c_str(), &statbuf) == 0) {
                // Format last modified time
                char timeStr[100];
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", 
                        localtime(&statbuf.st_mtime));

                // Add row to table
                html << "<tr><td><a href=\"" << name;
                if (S_ISDIR(statbuf.st_mode))
                    html << "/";
                html << "\">" << name;
                if (S_ISDIR(statbuf.st_mode))
                    html << "/";
                html << "</a></td><td>" << timeStr << "</td><td>";
                if (!S_ISDIR(statbuf.st_mode))
                    html << statbuf.st_size;
                html << "</td></tr>\n";
            }
        }
        closedir(dir);
    }

    html << "</table>\n</body>\n</html>";
    return html.str();
}

//need to adjust to use right values
void	Response::generateHeader(HttpRequest &reqObj){
	generateStatusLine(reqObj);

	std::map<std::string, std::string> headerMap;
	headerMap["Status-Line"] = getStatusLine();
    headerMap["Date"] = getCurrentDateTime();
	headerMap["Server"] = getServerName();
	// Get the file extension from the PathInfo object
	std::cout << reqObj.getPathInfo() << '\n';
    std::string extension = reqObj.getPathInfo().getExtension();
    headerMap["Content-Type"] = getContentType(extension);

	headerMap["Content-Length"] =  intToString(getBody().length());
	headerMap["Connection"] = "keep alive";//example value set to this default but in reguest could ask for close

	// Optional Fields
    headerMap["Cache-Control"] = "no-cache"; // Example value
    headerMap["Set-Cookie"] = "sessionId=abc123; Path=/; HttpOnly"; // Example value
    headerMap["Last-Modified"] = getCurrentDateTime(); // Example value
    headerMap["ETag"] = "\"123456789\""; // Example value
    //headerMap["Location"] = "/new-resource"; // Example value

	setHeaderMap(headerMap);
}




void Response::generateErrorResponse(HttpRequest &reqObj){
	//check for custom error page saved by moritz somewhere
	 // Retrieve the error pages map
	 const std::map<int, std::string>& errorPages = _serverConf->getErrorPages();

	 // Loop over the error pages and print them
	 for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
		 std::cout << "Error Code: " << it->first << ", Error Page: " << it->second << std::endl;
	 }
	 
	 // Check if a custom error page exists for the response code
	 int responseCode = reqObj.getResponseCode();
	 std::map<int, std::string>::const_iterator customPageIt = errorPages.find(responseCode);
	 if (customPageIt != errorPages.end()) {
		 // Custom error page found, load and set it as the response body
		 std::string errorPagePath = "." + customPageIt->second; // Use relative path
		 std::ifstream file(errorPagePath.c_str());
		 if (file) {
			 std::ostringstream contents;
			 contents << file.rdbuf();
			 file.close();
			 setBody(contents.str());
			//updats the path to the one for the error pages 
			PathInfo errorPath(errorPagePath);
			errorPath.parsePath();
			reqObj.setPathInfo(errorPath);
			
		 } else {
			 std::cerr << "Failed to open custom error page: " << customPageIt->second << std::endl;
			 setBody("<html><body><h1>" + Response::intToString(responseCode) + " " + genarateReasonPhrase(reqObj) + " Error</h1></body></html>\n");
		 }
	 } else {
		 // Generate a default error page if custom error page is not found
		 std::string defaultErrorPage = "<html><body><h1>" + Response::intToString(responseCode) + " " + genarateReasonPhrase(reqObj) + " Error</h1></body></html>\n";
		 setBody(defaultErrorPage);
	 }
		// Generate a default error page if custom error page is not found

		generateHeader(reqObj);
}


void		Response::processResponse(HttpRequest &ReqObj){
    std::cout << std::endl << "IN RESPONSE PROCESSING" <<std::endl << std::endl;
	
	
	std::cout << "Current URI: " << ReqObj.getUri() << std::endl;
	if (_locationConf) {
		std::cout << "Matched Location: " << _locationConf->getPath() << std::endl;
	} else {
		std::cout << "No location configuration matched" << std::endl;
	}
	
	
	try{
		 PathInfo pathInfo = ReqObj.getPathInfo();
		 // Validate and parse the path
            int validationCode = pathInfo.validatePath();
            std::cout << "Validation Code: " << validationCode << std::endl;
            pathInfo.parsePath();
        	std::cout << "Parsed Path Information: " << pathInfo << std::endl;
        if (ReqObj.getMethod() == "GET") {
            std::cout << "GET REQUEST" << std::endl;

            std::cout << "Path Information: " << pathInfo << std::endl;
            if (pathInfo.isDirectory() || pathInfo.getFullPath() == ""){
				std::cout << "IS DIR" << std::endl;
             // Get the index file from server configuration
			    //std::string indexFile = _serverConf->getIndex();
    			std::vector<std::string> indexFile = _serverConf->getIndex();
				//std::cout << "index files"<< indexFile[0] << "\n";
				// Add this debug code after getting the index files
				std::cout << "Number of index files: " << indexFile.size() << std::endl;
				if (!indexFile.empty()) {
			    std::cout << "First index file: " << indexFile[0] << std::endl;
				}
				if (!indexFile.empty()) {
					// Use the first index file in the vector
					std::string fullPath = _serverConf->getRootDir() + indexFile[0];
					std::cout << "Trying to open file at: " << fullPath << std::endl;  // Debug line
					
					// For debugging, print all configured index files
					std::cout << "Available index files:" << std::endl;
					for (std::vector<std::string>::const_iterator it = indexFile.begin(); 
						 it != indexFile.end(); ++it) {
						std::cout << "- " << *it << std::endl;
					}
		    	// Try to open the index file
    			std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    			if (file) {
        		// Index file exists, serve it
        			std::ostringstream contents;
        			contents << file.rdbuf();
        			file.close();
        			setBody(contents.str());
					ReqObj.setResponseCode(200);
					std::cout << "Index file exists and opens" << std::endl;
        		// Update PathInfo to reflect the index file
       				PathInfo indexPath(fullPath);
        			indexPath.parsePath();
        			ReqObj.setPathInfo(indexPath);
    		}
			//else {
        	// Check if autoindex is enabled
        	 // If no index file and autoindex is on, show directory listing
			 if (_locationConf) {
				bool autoIndexEnabled = _locationConf->getAutoIndex();
				std::cout << "Auto indexing enabled: " << (autoIndexEnabled ? "true" : "false") << "\n";
				
				if (autoIndexEnabled) {
					// If autoindex is ON, show directory listing
					setBody(generateDirectoryListing(pathInfo.getFullPath()));
					ReqObj.setResponseCode(200);
					} 
				else{
					// If autoindex is OFF and no index file was found
					ReqObj.setResponseCode(403);
					throw 403;
					}
				}
				}
			}
            else if (pathInfo.isFile()) {
                std::cout << "Path is a file" << std::endl;
                std::string fullPath = pathInfo.getFullPath();
  				std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    			if (!file) {
        			setBody("<html><body><h1>404 Not Found</h1></body></html>");
    			}

    			std::ostringstream contents;
    			contents << file.rdbuf();
   				file.close();
                setBody(contents.str());
            } else {
                std::cerr << "Path is neither a file nor a directory" << std::endl;
                throw 404; // Not Found
            }
        }

        if (ReqObj.getMethod() == "POST") {
			std::cout << "POST REQUEST" << std::endl;
			// Get the Content-Length from the HTTP request headers
			// With this:
			    // First check if Content-Length header exists
				 // Get headers with correct type
				 std::map<std::string, std::vector<std::string> > headers = ReqObj.getHeaders();
				 std::map<std::string, std::vector<std::string> >::const_iterator it = headers.find("Content-Length");
				 
				 if (it == headers.end() || it->second.empty()) {
					 std::cerr << "Missing Content-Length header" << std::endl;
					 throw 411; // Length Required
				 }
				 
				 // Use the first value from the vector
				 std::istringstream iss(it->second[0]);
				 size_t contentLength;
				 iss >> contentLength;
				 
				 if (iss.fail()) {
					 std::cerr << "Failed to parse Content-Length header" << std::endl;
					 throw 400; // Bad Request
				 }

			// Determine the maximum allowed body size using ternary operator
			size_t maxBodySize = _serverConf->getBodySize();
			std::cout << "MAX BODY SIZE:" << maxBodySize << "CONTENT length :" << contentLength << "\n";
    		//_locationConf->getClientMaxBodySize() :  // If location config exists, use its limit->> dont have that parsed idk if we wna tto include 
    		//_serverConf->getClientMaxBodySize();     // Otherwise, use server's default limit
			
			// Check size before processing
            if (contentLength > maxBodySize) {
                ReqObj.setResponseCode(413); // Entity Too Large
				throw 413;
			}
			std::string postData = ReqObj.getBody();
			std::cout << "Received POST data: " << postData << std::endl;
		
			// Validate the POST data
			if (postData.empty()) {
				std::cerr << "POST data is empty" << std::endl;
				throw 400; // Bad Request
			}
			// Create upload directory if it doesn't exist
    		std::string uploadDir = "./uploads/";  // Start with relative path
    if (_locationConf && !_locationConf->getUploadDir().empty()) {
        uploadDir = "./" + _locationConf->getUploadDir();  // Add ./ prefix
        if (uploadDir[uploadDir.length() - 1] != '/') {
            uploadDir += "/";
        }
    }

    // Create directories recursively
    std::string dirPath = uploadDir;
    size_t pos = 0;
    while ((pos = dirPath.find('/', pos + 1)) != std::string::npos) {
        std::string subPath = dirPath.substr(0, pos);
        if (mkdir(subPath.c_str(), 0755) == -1 && errno != EEXIST) {
            std::cerr << "Failed to create directory: " << subPath << std::endl;
            throw 500;
        }
    }

    // Generate unique filename with proper path
    std::string filePath = uploadDir + "upload_" + getCurrentDateTime();
    std::string::size_type slashPos = 0;
    while ((slashPos = filePath.find_first_of(" :", slashPos)) != std::string::npos) {
        filePath[slashPos] = '_';
    }
		
			std::cout << "Saving to path: " << filePath << std::endl;
			
			std::ofstream outFile(filePath.c_str(), std::ios::out | std::ios::binary);
			if (!outFile.is_open()) {
    		std::cerr << "Failed to open file: " << filePath << std::endl;
    		throw 500; // Internal Server Error
			}

			// Write the POST data to file
			outFile.write(postData.c_str(), postData.length());
			outFile.close();

			if (outFile.fail()) {
    		std::cerr << "Failed to write to file: " << filePath << std::endl;
    		throw 500; // Internal Server Error
			}
		
			// Generate a success response
			setBody("<html><body><h1>POST Data Received</h1></body></html>");
		}


        // Handle DELETE request
        if (ReqObj.getMethod() == "DELETE") {
            std::cout << "DELETE REQUEST" << std::endl;
            std::string fullPath = pathInfo.getFullPath();
            if (pathInfo.isFile()) {
                if (std::remove(fullPath.c_str()) == 0) {
                    std::cout << "File deleted successfully: " << fullPath << std::endl;
                    setBody("<html><body><h1>File Deleted Successfully</h1></body></html>");
                } else {
                    std::cerr << "Failed to delete file: " << fullPath << std::endl;
                    throw 500; // Internal Server Error
                }
            } else {
                std::cerr << "Path is not a file or does not exist: " << fullPath << std::endl;
                throw 404; // Not Found
            }
        }



    }
    catch(int error)
    {
		generateErrorResponse(ReqObj);
        //add the error handeling here
        //make error body and send that string to client
    }

    //return the string i gonna send to the client
}








std::string Response::intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}





std::string Response::genarateReasonPhrase(HttpRequest &reqObj){
	std::string statusCode = Response::intToString(reqObj.getResponseCode());
	std::string reasonPhrase;
	switch (reqObj.getResponseCode()){
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
	setReasonPhrase(reasonPhrase);
	return(reasonPhrase);
}
