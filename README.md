# WEBSERV -- A lightweight HTTP/1.1 Server in C++98

## OVERVIEW

A simple HTTP/1.1 webserver which handles GET, POST and DELETE Requests. Servers Files and handles CGI. The connentions get handled via epoll and is a non-blocking I/O structure. The Server implements Cookies aswell as Sessions and is capable of handling any CGI script. 

## FEATURES

- Feature 1 - concurrent Client handeling, up to tens of thouands and a time complexity of O(1) thanks to epoll

- Feature 2 - Full HTTP/1.1 support, handles GET, POST, DELETE request, keep-alive connections and multidata uploads

- Feature 3 - Fileserving, can serve static files aswell as directory listing

- Feature 4 - CGI execution, can handle multiple CGI scripts and languages aswell as errors and timeouts

- Feature 5 - Session Management, server is capable of registering Cookies and Sessions and handles Clients accordingly

- Feature 6 - Virtual hostname hosting, capable to host multiple Servers under one Ip:Port combo

- Feature 7 - Locaiton Configuration, handles different locations according to the conf file

- Feature 8 - custom conf file

- Feature 9 - generates basic error pages if no specific error pages defined 

## HOW TO RUN

```bash
git clone git@github.com:alecmalloc/webserve.git 
cd webserv
make 
./webserv conf/default.conf
```

## CONF FILE SETUP

```
Possible Setups:

- different Ip:Ports                            -> listen               127.0.0.1:8080;
- different Hostnames                           -> server_name          localhost;
- define Errorpages                             -> error_page           404 404.html;
- define Client max bodysize                    -> client_max_body_size 1g;
- define root Directory                         -> root                 ./;
- define default index file                     -> index                assets/index.html;
- allow auto Directory listing                  -> autoindex            on;
- allow chunked transfer                        -> use_chunked_encoding on;
- define chunk size                             -> chunk_size           1m;
- define default redirects                      -> allowed_redirects    301 www.google.com;
- different Locations                           -> location / {}
    - allow different Methods                   -> allowed_methods      GET POST DELETE;
    - define lcoation specific redirects        -> allowed_redirects    302 www.google.com;
    - define location specific root directory   -> root                 ./location;
    - allow auto directory listing              -> autoindex            on;
    - define location specific index file       -> index                assets/index.html;
    - define cgi executable endings             -> cgi_ext              .py .pl;
    - define allowed cgi executables            -> cgi_path             /usr/bin/python3;
    - define a specified upload directory       -> upload_dir           uploads;
    - define a location client body size        client_max_body_size    1;
```

## Project Structure

```plaintext
/
├── src                            -> source files                   
│    ├── Cgi                       -> Cgi handling
│    ├── Config                    -> Config parsing
│    ├── Http                      -> Request parsing
│    ├── Response                  -> Resonse Handling
│    ├── Server                    -> Server Initilastion and mainLoop
│    └── main.cpp
├── inc                            -> include files
├── server
│    ├── error_page                -> store custom error pages
│    ├── assets                    -> stores jpeg, gifs, pdf et
│    ├── conf                      -> stores conf files
│    ├── cgi-bin                   -> stores cgi scripts python
│    │    └── uploads                   -> cgi upload directory
│    ├── html                      -> stores html files
│    └── uploads                   -> default upload directory
├── conf                           -> stores conf files
├── Makefile
└── README.md

```

## AUTHORS

@alecmalloc @eschencode @bartsch-tech
