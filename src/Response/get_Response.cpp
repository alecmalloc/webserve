#include "webserv.hpp"

void Response::HandleGetRequest(HttpRequest& ReqObj, PathInfo& pathInfo){
	std::cout << "Path Information: " << pathInfo << std::endl;
            if (pathInfo.isDirectory() || pathInfo.getFullPath() == ""){
    			std::vector<std::string> indexFile = _serverConf->getIndex();
				if (!indexFile.empty()) {
					// Use the first index file in the vector
					std::string fullPath = _serverConf->getRootDir() + indexFile[0];
		    	// Try to open the index file
    			std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
    			if (file) {
        		// Index file exists, serve it
        			std::ostringstream contents;
        			contents << file.rdbuf();
        			file.close();
        			setBody(contents.str());
					ReqObj.setResponseCode(200);

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