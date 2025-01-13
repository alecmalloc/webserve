#ifndef PATH_HPP
#define PATH_HPP

#include <string>
#include <sys/stat.h> // file stat to check existence and properties

// ALL FILE OPERATIONS FOR SERVER

class PathInfo {
    private:
        std::string m_fullPath; // /home/users/report.txt
        // TODO: parse these out of full path
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
        void parsePath();
    
    public:
        PathInfo(const std::string& path);

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

// url to file system path
bool resolvePath(const std::string& urlPath, std::string& fsPath);
    // Convert URL path to filesystem path
    // Check for directory traversal attempts
    // Validate final path is within server root

// handle directory requests
// Response handleDirectory()
    // Check for index files (index.html, etc)
    // If index exists -> serve file
    // If no index and listings enabled -> generate listing
    // If listings disabled -> 403 Forbidden

// serve static files
// Response serveFile()
    // Open file
    // Get file size
    // Determine MIME type
    // Read file content
    // Generate response headers
    // Return response with content

// handle common errors?
// Response handleFileError(FileError Error)

#endif