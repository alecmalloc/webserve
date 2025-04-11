#include "webserv.hpp"

void Response::HandleGetRequest(HttpRequest& ReqObj, PathInfo& pathInfo) {

    // get uri from request
    std::string uri = ReqObj.getUri();

    // custom cookies override - Alec
    // check if the uri points at the /customCookiesEndpoint/CookiesPage end point
    if (uri == "/customCookiesEndpoint/CookiesPage" 
        || uri == "/customCookiesEndpoint/CookiesPage/deactivate" 
        || uri == "/customCookiesEndpoint/CookiesPage/activate") {
        handleCookiesPage(ReqObj);
        return;
    }

    // std::cout << "Path Information: " << pathInfo << std::endl;
    std::string fullPath = pathInfo.getFullPath();
	// std::cout << "FULL PATH" << fullPath << "IN GET HANDLER START \n";
    if (pathInfo.isDirectory() || pathInfo.getFullPath() == "") {
        
        // Construct the proper path without duplication
        std::string fullPath = _serverConf->getRootDir();
        
        if (_locationConf && uri.find(_locationConf->getPath()) == 0) {
            // URI already starts with location path, just use the URI as-is
            fullPath = _serverConf->getRootDir() + uri;
            std::cout << "Path constructed from URI only: " << fullPath << std::endl;
        } else if (_locationConf) {
            // Location doesn't match URI prefix, need to combine them
            fullPath = _serverConf->getRootDir() + _locationConf->getPath() + uri;
            std::cout << "Path constructed from location + URI: " << fullPath << std::endl;
        } else {
            // No location matched
            fullPath = _serverConf->getRootDir() + uri;
            // std::cout << "No location matched, path: " << fullPath << std::endl;
        }
        
        std::vector<std::string> indexFiles = _serverConf->getIndex();
        if (!indexFiles.empty()) {
            // Use the first index file in the vector
            std::string indexFilePath = pathInfo.getDirName() + indexFiles[0];
            
            // Try to open the index file - FIXED: using indexFilePath instead of fullPath
            // std::cout << "Trying to open index file: " << indexFilePath << std::endl;
            std::ifstream file(indexFilePath.c_str(), std::ios::in | std::ios::binary);
            
            if (file) {
                // Index file exists, serve it
                std::ostringstream contents;
                contents << file.rdbuf();
                file.close();
                setBody(contents.str());
                ReqObj.setResponseCode(200);

                // Update PathInfo to reflect the index file
                PathInfo indexPath(indexFilePath);
                indexPath.parsePath();
                ReqObj.setPathInfo(indexPath);
                return; // Exit after successfully serving index file
            }
        }
        
        // If we reach here, no index file was found or successfully opened
        if (_locationConf) {
			std::cout << "AUTO INDEXING \n";
            bool autoIndexEnabled = _locationConf->getAutoIndex();
            if (autoIndexEnabled) {
                // If autoindex is ON, show directory listing
                setBody(generateDirectoryListing(pathInfo.getFullPath()));
                ReqObj.setResponseCode(200);
            } else {
                // If autoindex is OFF and no index file was found
                ReqObj.setResponseCode(403);
                std::cout << "$$$$$$$$$$$$$$$$$403\n"; 
                throw 403;
            }
        } else {
            // No location matched and no index file, show 404
            ReqObj.setResponseCode(404);
            throw 404;
        }
    }
    else if (pathInfo.isFile()) {
        // std::cout << "Path is a file" << std::endl;
        std::string fullPath = pathInfo.getFullPath();
        std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);
        
        if (!file) {
            setBody("<html><body><h1>404 Not Found</h1></body></html>");
            ReqObj.setResponseCode(404);
            throw 404;
        }

        std::ostringstream contents;
        contents << file.rdbuf();
        file.close();
        setBody(contents.str());
        ReqObj.setResponseCode(200);
    } else {
        std::cerr << "Path is neither a file nor a directory" << std::endl;
        ReqObj.setResponseCode(404);
        throw 404; // Not Found
    }
}