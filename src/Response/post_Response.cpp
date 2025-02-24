#include "webserv.hpp"
#include <ctime>


int Response::checkContentLength(HttpRequest& ReqObj){
	std::map<std::string, std::vector<std::string> > headers = ReqObj.getHeaders();
	std::map<std::string, std::vector<std::string> >::const_iterator it = headers.find("Content-Length");
		 
	if (it == headers.end() || it->second.empty()) {
		std::cerr << "Missing Content-Length header" << std::endl;
		return (411); // Length Required
	}
		 
		 // Use the first value from the vector
		 std::istringstream iss(it->second[0]);
		 size_t contentLength;
		 iss >> contentLength;
		 
		 if (iss.fail()) {
			 std::cerr << "Failed to parse Content-Length header" << std::endl;
			 return 400; // Bad Request
		 }

	// Determine the maximum allowed body size using ternary operator
	size_t maxBodySize = _serverConf->getBodySize();
	std::cout << "MAX BODY SIZE:" << maxBodySize << "CONTENT length :" << contentLength << "\n";
	//_locationConf->getClientMaxBodySize() :  // If location config exists, use its limit->> dont have that parsed idk if we wna tto include 
	//_serverConf->getClientMaxBodySize();     // Otherwise, use server's default limit
	
	// Check size before processing
	if (contentLength > maxBodySize) {
		//ReqObj.setResponseCode(413); // Entity Too Large
		return 413;
	}
	return(200);
}


void Response::HandlePostRequest(HttpRequest& ReqObj,PathInfo& pathinfo){
	std::cout << "POST REQUEST" << std::endl;
	(void)pathinfo;
	(void)ReqObj;
	
	if(checkContentLength(ReqObj) != 200)
		throw(checkContentLength(ReqObj));
	std::string postData = ReqObj.getBody();
	std::cout << "Received POST data: " << postData << std::endl;
	
	std::string contentType = ReqObj.getHeaders()["Content-Type"][0];
	std::cout << "Content-Type" << contentType << "\n";

	// Validate the POST data
	if (postData.empty()) {
		std::cerr << "POST data is empty" << std::endl;
		throw 400; // Bad Request
	}

	//  // Create upload directory if it doesn't exist
	std::string filePath = uploadPathhandler(_locationConf);
	std::cout << "FILE PATH:" << filePath << ":  \n";
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
	if (!outFile.fail()) {
		ReqObj.setResponseCode(201);
		setStatusCode(201);

		// Set success response body
		setBody("<html><body>"
				"<h1>Upload Successful</h1>"
				"<p>Your file has been uploaded successfully.</p>"
				"<a href='/'>Return to home</a>"
				"</body></html>");
	}
}

std::string Response::uploadPathhandler(LocationConf* locationConf) {
	std::string uploadDir = "./uploads/"; // Default upload directory

	if (locationConf && !locationConf->getUploadDir().empty()) {
		uploadDir = "./" + locationConf->getUploadDir(); // Make path relative
		
		// Make sure upload dir ends with a slash
		if (uploadDir[uploadDir.length() - 1] != '/') {
			uploadDir += "/";
		}
	}

	// Create directory recursively
	std::string dirPath = uploadDir;
	size_t pos = 0;
	while ((pos = dirPath.find('/', pos + 1)) != std::string::npos) {
		std::string subPath = dirPath.substr(0, pos);
		if (!subPath.empty() && access(subPath.c_str(), F_OK) == -1) {
			if (mkdir(subPath.c_str(), 0777) == -1) {
				std::cerr << "Failed to create directory: " << subPath << std::endl;
				throw 500; // Internal Server Error
			}
		}
	}

	// Verify directory exists and has write permissions
	if (access(uploadDir.c_str(), W_OK) == -1) {
		std::cerr << "No write permission for directory: " << uploadDir << std::endl;
		throw 403; // Forbidden
	}

	// Generate unique filename with proper path
	std::string filePath = uploadDir + "upload_" + getCurrentDateTime();
	
	// Replace spaces and colons in filename with underscores
	std::string::size_type position = 0;
	while ((position = filePath.find_first_of(" :", position)) != std::string::npos) {
		filePath[position] = '_';
		position++; // Move to next character
	}
		
	std::cout << "Saving to path: " << filePath << std::endl;
	return filePath;
}
