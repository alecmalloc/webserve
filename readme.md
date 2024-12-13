# WebServ Project Task Distribution

## Moritz (Core Architecture)
- TCP connection management and socket handling
- I/O multiplexing with select()
- Main event loop implementation
- Server block parsing
- Location block interpretation
- Process management for CGI

**Integration Points:** Will need to work closely with others on request handling and CGI implementation

## Alec
- HTTP request parsing
- Method validation
- Header extraction and processing
- Static file handler
- File system operations
- MIME type detection
- Directory listing

**Integration Points:** Will coordinate with Moritz on request handling and Linus for error responses

## Linus
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
- Configuration file parsing
- Testing and debugging
- Documentation
- Code reviews

---

This distribution ensures each person has a clear area of responsibility while maintaining logical connections between related components. Moritz handles the core architecture, Alec manages request processing and file operations, and Linus takes care of responses and error handling.
