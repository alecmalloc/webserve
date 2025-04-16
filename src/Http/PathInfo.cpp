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
    _dirName(""),
    _baseName(""),
    _extension(""),
    _fileName(""),
    _isDirectory(false),
    _isFile(false),
    _hasExtension(false)
{
    ;
}

PathInfo::PathInfo(const std::string& path):
    _fullPath(path),
    _dirName(""),
    _baseName(""),
    _extension(""),
    _fileName(""),
    _isDirectory(false),
    _isFile(false),
    _hasExtension(false)
{
    ;
}

PathInfo::PathInfo(const PathInfo& other):
    _fullPath(other._fullPath),
    _dirName(other._dirName),
    _baseName(other._baseName),
    _extension(other._extension),
    _fileName(other._fileName),
    _isDirectory(other._isDirectory),
    _isFile(other._isFile),
    _hasExtension(other._hasExtension)
{
    ;
}

PathInfo& PathInfo::operator=(const PathInfo& other) {
    if (this != &other) {
        _fullPath = other._fullPath;
        _dirName = other._dirName;
        _baseName = other._baseName;
        _extension = other._extension;
        _isDirectory = other._isDirectory;
        _isFile = other._isFile;
        _hasExtension = other._hasExtension;
    }
    return *this;
}

PathInfo::~PathInfo() {

}

int PathInfo::validatePath() {
    // First parse the path components - do this before validation
    parsePath();

    // Check for path traversal attempts using ".." in path
    if (_fullPath.find("..") != std::string::npos)
        return 403; // Forbidden - security risk

    // Create stat structure to store file information
    struct stat statbuf;

    // Try to get file status
    if (stat(_fullPath.c_str(), &statbuf) != 0) {
        // Even if file doesn't exist, we keep the parsed path information
        _isDirectory = false;
        _isFile = false;
        //return 404; // Not Found
    }

    // Set file type flags based on stat results
    _isDirectory = S_ISDIR(statbuf.st_mode);
    _isFile = S_ISREG(statbuf.st_mode);

    // Additional directory check using opendir
    if (_isDirectory) {
        DIR* dir = opendir(_fullPath.c_str());
        if (dir == NULL) {
            _isDirectory = false;
            return 404;
        }
        closedir(dir);

        // Check directory permissions
        if (!(statbuf.st_mode & S_IRUSR) || !(statbuf.st_mode & S_IXUSR)) {
            return 403;
        }
    } else if (_isFile) {
        // Check file permissions
        if (!(statbuf.st_mode & S_IRUSR)) {
            return 403;
        }
    }

    return 200;
}

int PathInfo::parsePath() {
    // Always parse and set path components regardless of validation
    size_t lastSlash = _fullPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        _dirName = _fullPath.substr(0, lastSlash);
        _fileName = _fullPath.substr(lastSlash + 1);
        _baseName = _fileName;  // Store the base name before extension processing
    } else {
        _dirName = "";
        _fileName = _fullPath;
        _baseName = _fullPath;
    }

    // Process extension
    size_t lastDot = _fileName.find_last_of('.');
    if (lastDot != std::string::npos) {
        _extension = _fileName.substr(lastDot + 1);
        _baseName = _fileName.substr(0, lastDot);
        _hasExtension = true;
    } else {
        _extension = "";
        _hasExtension = false;
    }

    return 200;
}

std::ostream& operator<<(std::ostream& os, const PathInfo& request) {
    os << "PathInfo {\n"
    << "  Full Path: " << request.getFullPath() << "\n"
    << "  Directory: " << request.getDirName() << "\n"
    << "  File Name: " << request.getFilename() << "\n"
    << "  Base Name: " << request.getBaseName() << "\n"
    << "  Extension: " << request.getExtension() << "\n"
    << "  Is Directory: " << (request.isDirectory() ? "true" : "false") << "\n"
    << "  Is File: " << (request.isFile() ? "true" : "false") << "\n"
    << "  Has Extension: " << (request.hasExtension() ? "true" : "false") << "\n"
    << "}";
    return os;
}
