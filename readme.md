# WebServ
### HTTP/1.1 compliant web server in C++98

<div align="center">
  <img src="assets/webserver.png" alt="webserve" width="150"/>
</div>

## Team Members
- [Moritz](https://github.com/mobartsch)
- [Alec](https://github.com/alecmalloc)
- [Linus](https://github.com/eschencode )


## Documentation
- [MIME Types](https://developer.mozilla.org/en-US/docs/Web/HTTP/MIME_types#structure_of_a_mime_type)
- [HTTP Response Codes](./assets/responseCodes.md)

## Project Task Distribution

### Moritz (Core Architecture)
- TCP connection management and socket handling
- I/O multiplexing with select()
- Main event loop implementation
- Server block parsing
- Location block interpretation
- Process management for CGI
- Configuration file parsing

**Integration Points:** Will need to work closely with others on request handling and CGI implementation

### Alec
- HTTP request parsing
- Method validation
- Header extraction and processing
- Static file handler
- File system operations
- MIME type detection
- Directory listing

**CGI variables**
DOCUMENT_ROOT: The root directory of your server.
HTTP_COOKIE: The visitor’s cookie, if one is set.
HTTP_HOST: The hostname of the page being attempted.
HTTP_REFERER: The URL of the page that called your program.
HTTP_USER_AGENT: The browser type of the visitor.
HTTPS: “on” if the program is being called through a secure server.
PATH: The system path your server is running under.
QUERY_STRING: The query string (see GET, below).
REMOTE_ADDR: The IP address of the visitor.
REMOTE_HOST: The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again).
REMOTE_PORT: The port the visitor is connected to on the web server.
REMOTE_USER: The visitor’s username (for .htaccess-protected pages).
REQUEST_METHOD: GET or POST.
REQUEST_URI: The interpreted pathname of the requested document or CGI (relative to the document root).
SCRIPT_FILENAME: The full pathname of the current CGI.
SCRIPT_NAME: The interpreted pathname of the current CGI (relative to the document root).
SERVER_ADMIN: The email address for your server’s webmaster.
SERVER_NAME: Your server’s fully qualified domain name (e.g., www.cgi101.com).
SERVER_PORT: The port number your server is listening on.
SERVER_SOFTWARE: The server software you’re using (e.g., Apache 1.3).

- [HTTP] (https://developer.mozilla.org/en-US/docs/Web/HTTP)

**Integration Points:** Will coordinate with Moritz on request handling and Linus for error responses

### Linus
- HTTP response construction
- Status code and headers generation
- Content formatting
- Error response handling
- CGI environment management
- CGI I/O handling
- Error management system
- Custom error pages

- maximum max body size location specific _locationConf->getClientMaxBodySize() for upload kinda like this IDK if nessary ?
- Make for post extraction of content if multi line dont know if nessary tho ?
**Integration Points:** Will need to work with Moritz on CGI implementation and Alec for request/response flow

## Shared Tasks
- Testing and debugging
- Documentation
- Code reviews

---

Moritz handles the core architecture, Alec manages request processing and file operations, and Linus takes care of responses and error handling.
