#include "../inc/Files.hpp"
#include "../inc/HttpHelpers.hpp"

bool resolvePath(const std::string& urlPath, std::string& fsPath) {
    std::string decodedUrl = urlDecoder(urlPath);
    // convert path to file system path
    // add root directory to url path 
    
}