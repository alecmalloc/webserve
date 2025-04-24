#include "webserv.hpp"


int Response::checkContentLength(HttpRequest& ReqObj){
	std::map<std::string, std::vector<std::string> > headers = ReqObj.getHeaders();

	std::map<std::string, std::vector<std::string> >::const_iterator transferEncodingIt = headers.find("Transfer-Encoding");
    if (transferEncodingIt != headers.end() && !transferEncodingIt->second.empty() &&
        transferEncodingIt->second[0] == "chunked") {
        //std::cout << "Chunked transfer encoding detected. Skipping Content-Length check.\n";
        return 200; // OK
    }

	std::map<std::string, std::vector<std::string> >::const_iterator it = headers.find("Content-Length");
	if (it == headers.end() || it->second.empty()) {
		throw 411;
	}

	// Use the first value from the vector
	std::istringstream iss(it->second[0]);
	size_t contentLength;
	iss >> contentLength;

	if (iss.fail()) {
		throw 400;
	}
	size_t maxBodySize = _serverConf->getBodySize();
			//_locationConf->getClientMaxBodySize() :  // If location config exists, use its limit->> dont have that parsed idk if we wna tto include
			//_serverConf->getClientMaxBodySize();     // Otherwise, use server's default limit
	if (contentLength > maxBodySize) {
		throw 413;
	}
	// default
	return(200);
}

void Response::genarateUploadSucces(HttpRequest& ReqObj){
	ReqObj.setResponseCode(201);
	setStatusCode(201);

	// Set success response body
	setBody("<html><body>"
			"<h1>Upload Successful</h1>"
			"<p>Your file has been uploaded successfully.</p>"
			"<a href='/'>Return to home</a>"
			"</body></html>");
}

void Response::HandlePostRequest(HttpRequest& ReqObj,PathInfo& pathinfo){
	// We dont always need content length in a post request. esp if chunked we use transfer-encoding instead
	std::string postData = ReqObj.getBody();

	std::string contentType = ReqObj.getHeaders()["Content-Type"][0];
	// std::cout << "Content-Type" << contentType << "\n";
	checkContentLength(ReqObj);

	if (contentType.find("multipart/form-data") != std::string::npos) {
		// Extract boundary
		size_t boundaryPos = contentType.find("boundary=");
		if (boundaryPos == std::string::npos) {
			throw 400; // Bad Request
		}
		std::string boundary = contentType.substr(boundaryPos + 9); // "boundary=" is 9 chars
		return handleMultipartUpload(ReqObj, pathinfo, boundary);
	}

	if (postData.empty()) { // succses on empty upload
		generateUploadSucces(ReqObj);
		return ;
	}

	//  // Create upload directory if it doesn't exist
	std::string filePath = uploadPathhandler(_locationConf);
	std::ofstream outFile(filePath.c_str(), std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		throw 500; // Internal Server Error
	}

	// Write the POST data to file
	outFile.write(postData.c_str(), postData.length());
	outFile.close();

	if (outFile.fail()) {
		throw 500; // Internal Server Error
	}
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
		genarateUploadSucces(ReqObj);
	}
}

std::string Response::uploadPathhandler(const LocationConf* locationConf) {
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
				throw 500; // Internal Server Error
			}
		}
	}

	// Verify directory exists and has write permissions
	if (access(uploadDir.c_str(), W_OK) == -1) {
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

	return filePath;
}








void Response::handleMultipartUpload(HttpRequest& ReqObj, PathInfo& pathinfo, const std::string& boundary) {
	(void)pathinfo;
	const std::string& postData = ReqObj.getBody();

    // Chrome may or may not include leading "--" in the actual data
    std::string boundaryMarker = "--" + boundary;
    std::string altBoundaryMarker = boundary; // Try without --

    // Find the start of the first part with either format
    size_t partStart = postData.find(boundaryMarker);
    if (partStart == std::string::npos) {
        partStart = postData.find(altBoundaryMarker);
        if (partStart == std::string::npos) {
            throw 400;
        }
    }

    // Skip to the end of the boundary line
    partStart = postData.find("\r\n", partStart);
    if (partStart == std::string::npos) {
        throw 400;
    }
    partStart += 2; // Skip \r\n

    // Find the Content-Disposition header
    size_t dispPos = postData.find("Content-Disposition:", partStart);
    if (dispPos == std::string::npos) {
        throw 400;
    }

    // Extract filename
    size_t filenamePos = postData.find("filename=\"", dispPos);
    if (filenamePos == std::string::npos) {
        throw 400;
    }
    filenamePos += 10; // Skip 'filename="'

    size_t filenameEnd = postData.find("\"", filenamePos);
    if (filenameEnd == std::string::npos) {
        throw 400;
    }

    std::string filename = postData.substr(filenamePos, filenameEnd - filenamePos);

    // Find the blank line that marks the start of file content
    size_t contentStart = postData.find("\r\n\r\n", filenameEnd);
    if (contentStart == std::string::npos) {
        throw 400;
    }
    contentStart += 4; // Skip \r\n\r\n

    // Find the end boundary
    size_t contentEnd = postData.find("\r\n--" + boundary, contentStart);
    if (contentEnd == std::string::npos) {
        throw 400;
    }

    // Extract file content
    std::string fileContent = postData.substr(contentStart, contentEnd - contentStart);

    // Create the upload directory (use your existing function)
    std::string uploadDir = uploadPathhandler(_locationConf);

    // But instead of using the generated filename, use the original filename
    // Extract the directory path from the uploadPathhandler result
    std::string dirPath = uploadDir.substr(0, uploadDir.find_last_of("/") + 1);

    // Use the original filename (sanitize it if needed)
    std::string filePath = dirPath + filename;

    // Write only the file content to the file
    std::ofstream outFile(filePath.c_str(), std::ios::out | std::ios::binary);
    if (!outFile.is_open()) {
        throw 500; // Internal Server Error
    }

    outFile.write(fileContent.c_str(), fileContent.length());
    outFile.close();

    if (outFile.fail()) {
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
