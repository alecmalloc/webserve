#include "webserv.hpp"

std::string Response::constructFullPath(const std::string uri){
	std::string fullPath = _serverConf->getRootDir();
	if (_locationConf && uri.find(_locationConf->getPath()) == 0) {
		// URI already starts with location path add uri
		fullPath +=  uri;
		//std::cout << "Path constructed from URI only: " << fullPath << std::endl;
	} else if (_locationConf) {
		// Location doesn't match URI prefix, need to combine them
		fullPath += _locationConf->getPath() + uri;
		//std::cout << "Path constructed from location + URI: " << fullPath << std::endl;
	} else {
		// No location matched
		fullPath += uri;
	}
	return(fullPath);
}


bool Response::serveFileIfExists(const std::string& fullPath, HttpRequest& ReqObj) {
    std::ifstream file(fullPath.c_str(), std::ios::in | std::ios::binary);

    if (file) {
        std::ostringstream contents;
        contents << file.rdbuf();
        file.close();

        setBody(contents.str());
        ReqObj.setResponseCode(200);

        return true; // File served successfully
    }

    return false; // File not found
}

bool Response::serveRootIndexfile(HttpRequest& ReqObj,std::string fullPath){
	std::vector<std::string> indexFiles = _serverConf->getIndex();
    if (!indexFiles.empty()) {
		for (std::vector<std::string>::iterator it = indexFiles.begin(); it != indexFiles.end(); ++it) {
			const std::string& indexFile = *it; // Dereference the iterator to get the current index file

			// Construct the path to the index file
			std::string rootIndexPath = _serverConf->getRootDir() + "/" + indexFile;

			if (serveFileIfExists(rootIndexPath, ReqObj) == true) {
				return true; // Exit after serving the index file
			}
		}
	}

	if(_serverConf->getAutoIndex() == true){
		setBody(generateDirectoryListing(fullPath));
		ReqObj.setResponseCode(200);
		return(true);
	}

	//no root index file
	ReqObj.setResponseCode(404);
	throw 404;
	return false;
}

bool Response::serveLocationIndex(HttpRequest& ReqObj){
	std::vector<std::string> indexFiles = _locationConf->getIndex();
	if (!indexFiles.empty()) {
		for (std::vector<std::string>::iterator it = indexFiles.begin(); it != indexFiles.end(); ++it) {
			const std::string& indexFile = *it; // Dereference the iterator to get the current index file

			// Construct the path to the index file
			std::string rootIndexPath = indexFile;
			//std::cout << "trying index file for location " << rootIndexPath << "\n";
			if (serveFileIfExists(rootIndexPath, ReqObj) == true) {
				//std::cout << "found index file for location " << rootIndexPath << "\n";
				return true; // Exit after serving the index file
			}
		}
	}
	return false;
}

void Response::HandleGetRequest(HttpRequest& ReqObj, PathInfo& pathInfo) {
    std::string uri = ReqObj.getUri();
	std::string fullPath = pathInfo.getFullPath();

    // custom cookies override - Alec
    // check if the uri points at the /customCookiesEndpoint/CookiesPage end point
    if (uri == "/customCookiesEndpoint/CookiesPage"
        || uri == "/customCookiesEndpoint/CookiesPage/deactivate"
        || uri == "/customCookiesEndpoint/CookiesPage/activate") {
        handleCookiesPage(ReqObj);
        return;
    }

	if(pathInfo.isFile()) {
		if(serveFileIfExists(fullPath, ReqObj) == true)
			return;
		else{
			ReqObj.setResponseCode(404);
			throw 404;
		}
	}

	//std::cout << "serverconf root dir " << _serverConf->getRootDir() << "and uri " << uri << "and full path " << fullPath <<"\n";
// Check if the URI is the root directory
	if (uri == "/" || fullPath == _serverConf->getRootDir() || fullPath == "./" || fullPath == "/") {
  		if (serveRootIndexfile(ReqObj, fullPath) == true)
        	return;
	}
    if (_locationConf) {

		if(pathInfo.isDirectory()){
			//check for location index file
		if(serveLocationIndex(ReqObj) == true){
			return;
		}
		std::cout << "loc conf auto index " << _locationConf->getAutoIndex() << " server conf auto " << _serverConf->getAutoIndex() << "\n";

  	  if( _locationConf->getAutoIndex() == true || (_serverConf->getAutoIndex() == true && _locationConf->getAutoIndex() == 0 )){
			setBody(generateDirectoryListing(pathInfo.getFullPath()));
			ReqObj.setResponseCode(200);
			return;
		}
		}
	}

    //std::cerr << "Path is neither a file nor a directory" << std::endl;
    ReqObj.setResponseCode(404);
    throw 404; // Not Found
}

