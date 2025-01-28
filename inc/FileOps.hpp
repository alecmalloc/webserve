#ifndef FILEOP_HPP
#define FILEOP_HPP

#include "webserv.hpp"

// not sure if i need a FileServer class rn, going to keep it simple until i need state management
Response serveDirectory(const PathInfo& path);
    // TODO
    // X 1. Check if Path is a Directory: Verify that the provided path is a directory.
    // X 2. Check for Default Files: Look for default files like index.html to serve.
    // 3. Check if Directory Listing is Enabled: Ensure that directory listing is allowed.
    // X 4. Read Directory Contents: List the contents of the directory if no default file is found.
    // 5. Generate Directory Listing HTML: Create an HTML page that lists the contents of the directory.
    // 7. Return Response: Return a Response object with the generated HTML content and appropriate status code.

Response serveFile(const PathInfo& path);
    // TODO
    // Open file
    // Get file size
    // Determine MIME type
    // Read file content
    // Generate response headers
    // Return response with content

// handle directory requests


// serve static files
// Response serveFile()


// handle common errors?
// Response handleFileError(FileError Error)


#endif