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

    public:
        PathInfo();
        PathInfo(const std::string& path);
        PathInfo(const PathInfo& other);
        PathInfo& operator=(const PathInfo& other);
        ~PathInfo();

        // not using str references as return due to no guarantee of use in lifecycle
        // getters
        std::string getFullPath() const;
        std::string getDirName() const;
        std::string getBaseName() const;
        std::string getExtension() const;
        std::string getFilename() const;
        bool isDirectory() const;
        bool isFile() const;
        bool hasExtension() const;

        // setters
        void setFullPath(const std::string& path);
        void setDirName(const std::string& dirName);
        void setBaseName(const std::string& baseName); 
        void setExtension(const std::string& extension);
        void setFilename(const std::string& filename);
        void setIsDirectory(bool isDir);
        void setIsFile(bool isFile);

        int validatePath();


};

#endif