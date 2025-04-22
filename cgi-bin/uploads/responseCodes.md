# HTTP Response Codes

## 1xx: Informational
- **100 Continue**: The server has received the request headers, and the client should proceed to send the request body.
- **101 Switching Protocols**: The requester has asked the server to switch protocols and the server has agreed to do so.

## 2xx: Success
- **200 OK**: The request was successful.
- **201 Created**: The request was successful and a new resource was created.
- **202 Accepted**: The request has been accepted for processing, but the processing has not been completed.
- **204 No Content**: The server successfully processed the request, but is not returning any content.

## 3xx: Redirection
- **301 Moved Permanently**: The requested resource has been assigned a new permanent URI.
- **302 Found**: The requested resource resides temporarily under a different URI.
- **304 Not Modified**: The resource has not been modified since the version specified by the request headers.

## 4xx: Client Error
- **400 Bad Request**: The server could not understand the request due to invalid syntax.
- **401 Unauthorized**: The client must authenticate itself to get the requested response.
- **403 Forbidden**: The client does not have access rights to the content.
- **404 Not Found**: The server can not find the requested resource.
- **405 Method Not Allowed**: The request method is known by the server but is not supported by the target resource.

## 5xx: Server Error
- **500 Internal Server Error**: The server has encountered a situation it doesn't know how to handle.
- **501 Not Implemented**: The request method is not supported by the server and cannot be handled.
- **502 Bad Gateway**: The server, while acting as a gateway or proxy, received an invalid response from the upstream server.
- **503 Service Unavailable**: The server is not ready to handle the request.
- **505 Wrong HTTP Version**
