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
	header << "Cache-Control: " << headerMap["Cache-Control"] << "\r\n";
	header << "Set-Cookie: " << headerMap["Set-Cookie"] << "\r\n";
	header << "Last-Modified: " << headerMap["Last-Modified"] << "\r\n";
	header << "ETag: " << headerMap["ETag"] << "\r\n";
	//header << "Location: " << headerMap["Location"] << "\r\n";
	header << "\r\n"; // End of headers
	std::string responseString = header.str() + getBody();
					std::cout << "respsonse string :" << responseString << ";;;;;\n";
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

		std::cout << "no custom errror page make default one :\n";
		 // Generate a default error page if custom error page is not found
		 std::string defaultErrorPage = "<html><body><h1>" + Response::intToString(responseCode) + " " + genarateReasonPhrase(reqObj) + " Error</h1></body></html>\n";
		 setBody(defaultErrorPage);
	 }
		// Generate a default error page if custom error page is not found

		generateHeader(reqObj);
		
}





bool Response::isCgiRequest(const std::string& uri) {
	// Check if URI matches CGI patterns
	// 1. First check: Location config exists and CGI is enabled
	
	
	if (_locationConf && 
		!_locationConf->getCgiPath().empty() && 
		!_locationConf->getCgiExt().empty())  {
	
	std::cout << "Checking CGI request for URI: " << uri << std::endl;
	std::cout << "Full path: " << _locationConf->getRootDir() + uri << std::endl;
	
	// 2. Find file extension
	size_t dot = uri.rfind('.');  // Finds last occurrence of '.' in URI
	if (dot != std::string::npos) {
		// 3. Extract extension (e.g., ".php", ".py")
		std::string ext = uri.substr(dot);
		
		// 4. Get allowed CGI extensions from location config
		const std::vector<std::string>& cgiExts = _locationConf->getCgiExt();
		
		// 5. Check if extension is in allowed list
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
		 // Validate and parse the path
			int validationCode = pathInfo.validatePath();
			std::cout << "Validation Code: " << validationCode << std::endl;
			pathInfo.parsePath();
			std::cout << "Parsed Path Information: " << pathInfo << std::endl;
		
			//FIRST CHECK IF ITS CGI REQUEST 
		if(isCgiRequest(ReqObj.getUri())){//should be ok but some errro with cgi handler cant find files
			int result = handleCgi(ReqObj);
			std::cout << "CGI result" << result << "\n";
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
		generateErrorResponse(ReqObj);
	}

}












