#include "webserv.hpp"

std::string PathInfo::getFullPath() const {
   return _fullPath; 
}

std::string PathInfo::getDirName() const {
    return _dirName;
}

std::string PathInfo::getBaseName() const {
    return _baseName;
}

std::string PathInfo::getExtension() const {
    return _extension;
}

std::string PathInfo::getFilename() const {
    return _fileName;
}


bool PathInfo::isDirectory() const {
    return _isDirectory;
}


bool PathInfo::isFile() const {
    return _isFile;
}

bool PathInfo::hasExtension() const {
    return _hasExtension;
}

PathInfo::PathInfo(void):
    _fullPath(""),
    _isDirectory(false),
    _isFile(false),
    _hasExtension(false)
{
    ;
}

PathInfo::PathInfo(const std::string& path):
    _fullPath(path),
    _isDirectory(false),
    _isFile(false),
    _hasExtension(false)
{
    if (!validatePath())
        throw std::runtime_error("Invalid path detected");
    // parse all other path properties
    if (!parsePath())
        throw std::runtime_error("Invalid path detected");
}

PathInfo::~PathInfo() {
    
}

bool PathInfo::validatePath() {

    // check for empty path
    if (_fullPath.empty())
        return false;

    // check for invalid path traversal ie -> "../../"
    if (_fullPath.find("..") != std::string::npos)
        return false;

    struct stat statbuf;
    // check file existence
    if (stat(_fullPath.c_str(), &statbuf) != 0)
        return false;

    // set file properties
    _isDirectory = S_ISDIR(statbuf.st_mode);
    _isFile = S_ISREG(statbuf.st_mode);

    // check dir with open dir too
    if (_isDirectory) {
        DIR* dir = opendir(_fullPath.c_str());
        if (dir == NULL) {
            return false;
        }
    }

    // check permissions
    if (_isDirectory) {
        // directory needs read and execute permissions
        if (!(statbuf.st_mode & S_IRUSR) || !(statbuf.st_mode & S_IXUSR))
            return false;
    } else if (_isFile) {
        // regular file needs read permission
        if (!(statbuf.st_mode & S_IRUSR))
            return false;
    }

    return true;
}

bool PathInfo::parsePath() {

    // find the last directory separator
    size_t lastSlash = _fullPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        _dirName = _fullPath.substr(0, lastSlash);
        _fileName = _fullPath.substr(lastSlash + 1);
    } else {
        _fileName = _fullPath;
    }

    // find the last dot to get the file extension
    size_t lastDot = _fileName.find_last_of('.');
    if (lastDot != std::string::npos) {
        _extension = _fileName.substr(lastDot + 1);
        _fileName = _fileName.substr(0, lastDot);
    }

    // additional validation for directory and filename
    if (_dirName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/\\._-") != std::string::npos)
        return false;
    if (_fileName.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-") != std::string::npos) 
        return false;

    return true;
}