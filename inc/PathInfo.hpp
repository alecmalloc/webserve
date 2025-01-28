#ifndef PATHINFO_HPP
#define PATHINFO_HPP

#include "webserv.hpp"

// save path for operations on files
class PathInfo {
    private:
        std::string _fullPath; // /home/users/report.txt
        std::string _dirName;  // /home/users
        std::string _baseName; // report
        std::string _extension; // .txt
        std::string _fileName; // report.txt

        // used m_ prefix to avoid conflict with function names
        bool _isDirectory;
        bool _isFile;
        bool _hasExtension;

        // moved to private because we only need at beginning
        bool validatePath();
        bool parsePath();

    public:
        PathInfo(const std::string& path);
        ~PathInfo();

        // not using str references as return due to no guarantee of lifecycle
        std::string getFullPath() const;
        std::string getDirName() const;
        std::string getBaseName() const;
        std::string getExtension() const;
        std::string getFilename() const;
        bool isDirectory() const;
        bool isFile() const;
        bool hasExtension() const;
};

// TODO:
// Validate final path is within server root

#endif