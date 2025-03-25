//#include "../inc/Response.hpp"
#include "webserv.hpp"
#include <sstream>

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
		std::cout << "DEBUG - Checking location: '" << it->getPath() << "'" << std::endl;
		
		if (uri.find(locPath) == 0) {  // URI starts with location path
			if (locPath.length() > bestMatch.length()) {
				bestMatch = locPath;
				_locationConf = const_cast<LocationConf*>(&(*it));
			}
		}
	}
	std::cout << "URI: " << uri << std::endl;
	std::cout << "Server root: " << _serverConf->getRootDir() << std::endl;
	if (_locationConf) {
    	std::cout << "Location path: " << _locationConf->getPath() << std::endl;
    	std::cout << "Location root: " << _locationConf->getRootDir() << std::endl;
	}
	//std::cout << "Final path being used: " << pathInfo.getFullPath() << std::endl;
	processResponse(reqObj);
	generateHttpresponse(reqObj);
}



void Response::generateHttpresponse(HttpRequest &reqObj){
	if (getStatusCode() == 0) {
        setStatusCode(reqObj.getResponseCode());
    }
	generateHeader(reqObj);
	std::map<std::string, std::string> headerMap = getHeaderMap();
	std::stringstream header ;
	header << headerMap["Status-Line"] << "\r\n";
	header << "Date: " << headerMap["Date"] << "\r\n";
	header << "Server: " << headerMap["Server"] << "\r\n";
	header << "Content-Type: " << headerMap["Content-Type"] << "\r\n";
	header << "Content-Length: " << headerMap["Content-Length"] << "\r\n";
	header << "Connection: " << headerMap["Connection"] << "\r\n";
	//header << "Cache-Control: " << headerMap["Cache-Control"] << "\r\n";
	//header << "Set-Cookie: " << headerMap["Set-Cookie"] << "\r\n";
	//header << "Last-Modified: " << headerMap["Last-Modified"] << "\r\n";
	//header << "ETag: " << headerMap["ETag"] << "\r\n";
	//header << "Location: " << headerMap["Location"] << "\r\n";
	header << "\r\n"; // End of headers
	std::string responseString = header.str() + getBody();
	setHttpResponse(responseString);
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



void Response::generateErrorResponse(HttpRequest &reqObj){
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
	generateHeader(reqObj);
}





bool Response::isCgiRequest(const std::string& uri) {
    // Check if URI matches CGI patterns
    if (_locationConf && 
        !_locationConf->getCgiPath().empty() && 
        !_locationConf->getCgiExt().empty())  {

        // Strip query parameters by finding the first '?'
        std::string path = uri;
        size_t queryPos = path.find('?');
        if (queryPos != std::string::npos) {
            path = path.substr(0, queryPos);
        }
        
        // Find file extension in the path (without query parameters)
        size_t dot = path.rfind('.');
        if (dot != std::string::npos) {
            std::string ext = path.substr(dot);
            
            // Get allowed CGI extensions
            const std::vector<std::string>& cgiExts = _locationConf->getCgiExt();
            
            // Check if extension is allowed
            return (std::find(cgiExts.begin(), cgiExts.end(), ext) != cgiExts.end());
        }
    }
    return false;
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
		
		/* if (pathInfo.getFullPath().empty() && !ReqObj.getUri().empty()) {
            std::cout << "DEBUG: Empty PathInfo detected, reconstructing from URI" << std::endl;
            
            // Construct the correct path
            std::string uri = ReqObj.getUri();
            std::string fullPath;
            
            // Use location config if available
            if (_locationConf) {
                fullPath = _locationConf->getRootDir() + uri;
                
                // Normalize path (remove double slashes)
                size_t pos;
                while ((pos = fullPath.find("//")) != std::string::npos) {
                    fullPath.replace(pos, 2, "/");
                }
            } else {
                fullPath = "." + uri;  // Default to current directory
            }
            
            std::cout << "DEBUG: Reconstructed path: " << fullPath << std::endl;
            
            // Create new PathInfo with fixed path
            pathInfo = PathInfo(fullPath);
        }*/
	
		 // Validate and parse the path
			int code = pathInfo.validatePath();
			std::cout << "ERROR CODE AFTER VALIDATEING GGG " <<  code << ":PENUS\n";
			//pathInfo.parsePath();
			
		
		//FIRST CHECK IF ITS CGI REQUEST 
		if(isCgiRequest(ReqObj.getUri())){//should be ok but some errro with cgi handler cant find files
			int result = handleCgi(ReqObj);
			std::cout << "CGI result" << result << "\n";
			if(result == 0)
			{
				setBody(ReqObj.getCgiResponseString());
			}
			return;
		}

		if (ReqObj.getMethod() == "GET") {
			std::cout << "GET REQUEST" << std::endl;
			HandleGetRequest(ReqObj,pathInfo);
		}

		if (ReqObj.getMethod() == "POST") {
		HandlePostRequest(ReqObj, pathInfo);
		}

		// Handle DELETE request
		if (ReqObj.getMethod() == "DELETE") {
		std::cout << "DELETE REQUEST" << std::endl;
		HandleDeleteRequest(ReqObj, pathInfo);
		}
		
	}
	catch(int error)
	{
		ReqObj.setResponseCode(error);
		std::cout << "CODE " << ReqObj.getResponseCode() << ":\n";
		generateErrorResponse(ReqObj);
	}

}












