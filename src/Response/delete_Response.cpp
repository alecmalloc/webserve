#include "webserv.hpp"


void Response::HandleDeleteRequest(HttpRequest& ReqObj, PathInfo& pathInfo){
	(void)ReqObj;
	
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