# WebServ
### HTTP/1.1 compliant web server in C++98

<div align="center">
  <img src="assets/webserver.png" alt="webserve" width="150"/>
</div>

## Team Members
- [Moritz](https://github.com/mobartsch)
- [Alec](https://github.com/alecmalloc)
- [Linus](https://github.com/eschencode )

## Project Task Distribution

### Moritz (Core Architecture)
- TCP connection management and socket handling
- I/O multiplexing with select()
- Main event loop implementation
- Server block parsing
- Location block interpretation
- Process management for CGI

**Integration Points:** Will need to work closely with others on request handling and CGI implementation

### Alec
- Configuration file parsing
- HTTP request parsing
- Method validation
- Header extraction and processing
- Static file handler
- File system operations
- MIME type detection
- Directory listing

comments cheat sheet: TODO, DELETE

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

**Integration Points:** Will need to work with Moritz on CGI implementation and Alec for request/response flow

## Shared Tasks
- Testing and debugging
- Documentation
- Code reviews

---

Moritz handles the core architecture, Alec manages request processing and file operations, and Linus takes care of responses and error handling.
