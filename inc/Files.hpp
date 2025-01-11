#ifndef FILES_HPP
#define FILES_HPP

// ALL FILE OPERATIONS FOR SERVER

// common response return type?
// FileError error

// url to file system path
// bool resolvePath()
    // Remove URL encoding
    // Convert URL path to filesystem path
    // Check for directory traversal attempts
    // Validate final path is within server root


// check if path exists and type
// bool validatePath()
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