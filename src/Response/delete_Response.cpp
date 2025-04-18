#include "webserv.hpp"

void Response::HandleDeleteRequest(HttpRequest& ReqObj, PathInfo& pathInfo){
    // Check if DELETE method is allowed
	(void)ReqObj;
	if (!_locationConf) {
        std::cerr << "Location configuration is null, DELETE methode not allowed" << std::endl;
        throw 405; // MEthode not allowed 
    }
    const std::vector<std::string>& allowedMethods = _locationConf->getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), "DELETE") == allowedMethods.end()) {
        std::cerr << "DELETE method not allowed for this location" << std::endl;
        throw 405; // Method Not Allowed
    }

    // Get the full path of the file
    std::string fullPath = pathInfo.getFullPath();

    // Check if the path is a file
    if (pathInfo.isFile()) {
        // Check if we have permission to delete the file
        if (access(fullPath.c_str(), W_OK) != 0) {
            std::cerr << "Permission denied to delete file: " << fullPath << std::endl;
            throw 403; // Forbidden
        }

        // Attempt to delete the file
        if (std::remove(fullPath.c_str()) == 0) {
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