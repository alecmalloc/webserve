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
	return(202);
}

void Response::handleMultipartUpload(HttpRequest& ReqObj, PathInfo& pathinfo, const std::string& boundary) {
	(void)pathinfo;
	const std::string& postData = ReqObj.getBody();
	std::cout << "Body received, length: " << postData.length() << std::endl;
    // Debug: Print the first 50 bytes to verify content
    std::cout << "Body starts with: " << postData.substr(0, 50) << std::endl;
    
    // Chrome may or may not include leading "--" in the actual data
    std::string boundaryMarker = "--" + boundary;
    std::string altBoundaryMarker = boundary; // Try without --
    
    // Find the start of the first part with either format
    size_t partStart = postData.find(boundaryMarker);
    if (partStart == std::string::npos) {
        partStart = postData.find(altBoundaryMarker);
        if (partStart == std::string::npos) {
            std::cerr << "Invalid multipart format - boundary not found" << std::endl;
            throw 400;
        }
    }
    
    // Skip to the end of the boundary line
    partStart = postData.find("\r\n", partStart);
    if (partStart == std::string::npos) {
        std::cerr << "Invalid multipart format - no CRLF after boundary" << std::endl;
        throw 400;
    }
    partStart += 2; // Skip \r\n
    
    // Find the Content-Disposition header
    size_t dispPos = postData.find("Content-Disposition:", partStart);
    if (dispPos == std::string::npos) {
        std::cerr << "No Content-Disposition header found" << std::endl;
        throw 400;
    }
    
    // Extract filename
    size_t filenamePos = postData.find("filename=\"", dispPos);
    if (filenamePos == std::string::npos) {
        std::cerr << "No filename found in Content-Disposition" << std::endl;
        throw 400;
    }
    filenamePos += 10; // Skip 'filename="'
    
    size_t filenameEnd = postData.find("\"", filenamePos);
    if (filenameEnd == std::string::npos) {
        std::cerr << "Invalid filename format" << std::endl;
        throw 400;
    }
    
    std::string filename = postData.substr(filenamePos, filenameEnd - filenamePos);
    std::cout << "Extracted filename: " << filename << std::endl;
    
    // Find the blank line that marks the start of file content
    size_t contentStart = postData.find("\r\n\r\n", filenameEnd);
    if (contentStart == std::string::npos) {
        std::cerr << "Invalid multipart format - no content start" << std::endl;
        throw 400;
    }
    contentStart += 4; // Skip \r\n\r\n
    
    // Find the end boundary
    size_t contentEnd = postData.find("\r\n--" + boundary, contentStart);
    if (contentEnd == std::string::npos) {
        std::cerr << "Invalid multipart format - no content end" << std::endl;
        throw 400;
    }
    
    // Extract file content
    std::string fileContent = postData.substr(contentStart, contentEnd - contentStart);
    std::cout << "File content length: " << fileContent.length() << " bytes" << std::endl;
    std::cout << "File content: '" << fileContent << "'" << std::endl;
    
    // Create the upload directory (use your existing function)
    std::string uploadDir = uploadPathhandler(_locationConf);
    
    // But instead of using the generated filename, use the original filename
    // Extract the directory path from the uploadPathhandler result
    std::string dirPath = uploadDir.substr(0, uploadDir.find_last_of("/") + 1);
    
    // Use the original filename (sanitize it if needed)
    std::string filePath = dirPath + filename;
    std::cout << "Saving file to: " << filePath << std::endl;
    
    // Write only the file content to the file
    std::ofstream outFile(filePath.c_str(), std::ios::out | std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        throw 500; // Internal Server Error
    }
    
    outFile.write(fileContent.c_str(), fileContent.length());
    outFile.close();
    
    if (outFile.fail()) {
        std::cerr << "Failed to write to file: " << filePath << std::endl;
        throw 500; // Internal Server Error
    }
    
    // Success response
    ReqObj.setResponseCode(201);
    setStatusCode(201);
    
    // Set success response body with the actual filename
    setBody("<html><body>"
            "<h1>Upload Successful</h1>"
            "<p>Your file '" + filename + "' has been uploaded successfully.</p>"
            "<a href='/'>Return to home</a>"
            "</body></html>");
}


void Response::HandlePostRequest(HttpRequest& ReqObj,PathInfo& pathinfo){
	(void)pathinfo;
	(void)ReqObj;
	
	// We dont always need content length in a post request. esp if chunked we use transfer-encoding instead
	std::string postData = ReqObj.getBody();
	std::cout << "Received POST data: " << postData << std::endl;
	
	std::string contentType = ReqObj.getHeaders()["Content-Type"][0];
	std::cout << "Content-Type" << contentType << "\n";
	
	if (contentType.find("multipart/form-data") != std::string::npos) {
		std::cout << "HANDLING MULTIPART FORM DATA\n";
		
		// Extract boundary
		size_t boundaryPos = contentType.find("boundary=");
		if (boundaryPos == std::string::npos) {
			std::cerr << "No boundary found in Content-Type" << std::endl;
			throw 400; // Bad Request
		}
		
		std::string boundary = contentType.substr(boundaryPos + 9); // "boundary=" is 9 chars
		std::cout << "Boundary: " << boundary << std::endl;
		
		// Handle the multipart form data with the extracted boundary
		return handleMultipartUpload(ReqObj, pathinfo, boundary);
	}

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
		//setBody("<html><body>"
		//		"<h1>Upload Successful</h1>"
		//		"<p>Your file has been uploaded successfully.</p>"
		//		"<a href='/'>Return to home</a>"
		//		"</body></html>");
		setBody( "" );
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
