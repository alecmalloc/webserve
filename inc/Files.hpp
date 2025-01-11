#ifndef FILES_HPP
#define FILES_HPP

#include <string>

// ALL FILE OPERATIONS FOR SERVER

// TODO:
// Resonse return class
// FileError error class
// PathInfo class

class PathInfo {
    private:
        std::string fullPath; // /home/users/report.txt
        std::string dirName;  // /home/users
        std::string baseName; // report
        std::string extension; // .txt
        std::string fileName; // report.txt
    public:
        PathInfo(const std::string& path);

        // not using references as return due to no guarantee of lifecycle
        std::string getFullPath() const;
        std::string getDirName() const;
        std::string getBaseName() const;
        std::string getExtension() const;
        std::string getFilename() const;

        // path validation
        bool exists() const;
        bool isDirectory() const;
        bool isFile() const;
        bool isValidPath() const;
        bool hasExtension() const;

        // getRelativepath?
        // getAbsolutepath?
};

// url to file system path
bool resolvePath(const std::string& urlPath, std::string& fsPath);
    // Convert URL path to filesystem path
    // Check for directory traversal attempts
    // Validate final path is within server root

// check if path exists and type
bool validatePath(const std::string& path, PathInfo& info);
    // Check if path exists
    // Determine if file or directory
    // Check permissions (read/execute)
    // Return path information

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