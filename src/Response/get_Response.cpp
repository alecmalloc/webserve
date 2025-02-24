#include "webserv.hpp"

void Response::HandleGetRequest(HttpRequest& ReqObj, PathInfo& pathInfo){
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