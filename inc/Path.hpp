#ifndef PATH_HPP
#define PATH_HPP

#include <string>
#include <sys/stat.h> // file stat to check existence and properties


// save path for operations on files
class PathInfo {
    private:
        std::string m_fullPath; // /home/users/report.txt
        std::string m_dirName;  // /home/users
        std::string m_baseName; // report
        std::string m_extension; // .txt
        std::string m_fileName; // report.txt

        // used m_ prefix to avoid conflict with function names
        bool m_isDirectory;
        bool m_isFile;
        bool m_hasExtension;

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