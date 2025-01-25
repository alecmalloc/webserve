#include "webserv.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>

// Simulate testing environment
int main() {
    Config config; // Placeholder for server configuration
    HttpRequest req(1, config); // Instantiate HttpRequest with dummy file descriptor

    // Set up the request details
    req.setMethod( "POST" ); // HTTP method
    req.setUri( "./cgi-bin/test.cgi" ); // URI of the CGI script
    req.setUrl( "./cgi-bin/test.cgi" ); // Full URL of the CGI script
    req.setVersion( "HTTP/1.1" ); // HTTP version
    req.setBody( "name=Mofoker" ); // Body content for POST

    // Add headers
    req.setHeader( "Content-Type", "application/x-www-form-urlencoded" );
    req.setHeader( "Host", "localhost:8080" );
    req.setHeader( "User-Agent", "TestClient/1.0" );

    // Invoke the CGI handler
    std::cout << "Testing CGI handler..." << std::endl;
    int result = handleCgi(req);

    // Check the result
    if (result == 0) {
        std::cout << "CGI handler executed successfully." << std::endl;
    } else {
        std::cerr << "CGI handler failed with error code: " << result << std::endl;
    }

    return 0;
}

