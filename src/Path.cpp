#include "../inc/Path.hpp"
#include "../inc/HttpHelpers.hpp"

#include <stdexcept>

std::string PathInfo::getFullPath() const {
   return m_fullPath; 
}

std::string PathInfo::getDirName() const {
    return m_dirName;
}

std::string PathInfo::getBaseName() const {
    return m_baseName;
}

std::string PathInfo::getExtension() const {
    return m_extension;
}

std::string PathInfo::getFilename() const {
    return m_fileName;
}


bool PathInfo::isDirectory() const {
    return m_isDirectory;
}


bool PathInfo::isFile() const {
    return m_isFile;
}

bool PathInfo::hasExtension() const {
    return m_hasExtension;
}

PathInfo::PathInfo(const std::string& path):
    m_fullPath(path),
    m_isDirectory(false),
    m_isFile(false),
    m_hasExtension(false)
{
    if (!validatePath())
        throw std::runtime_error("Invalid path detected");
    // parse all other path properties
    parsePath();
}

bool PathInfo::validatePath() {

    // check for empty path
    if (m_fullPath.empty())
        return false;

    // check for invalid path traversal ie -> "../../"
    if (m_fullPath.find("..") != std::string::npos)
        return false;

    struct stat statbuf;
    // check file existence
    if (stat(m_fullPath.c_str(), &statbuf) == 0)
        return false;

    // set file properties
    m_isDirectory = S_ISDIR(statbuf.st_mode);
    m_isFile = S_ISREG(statbuf.st_mode);

    // check permissions
    if (m_isDirectory) {
        // directory needs read and execute permissions
        if (!(statbuf.st_mode & S_IRUSR) || !(statbuf.st_mode & S_IXUSR))
            return false;
    } else if (m_isFile) {
        // regular file needs read permission
        if (!(statbuf.st_mode & S_IRUSR))
            return false;
    }

    return true;
}

void PathInfo::parsePath() {
    
}