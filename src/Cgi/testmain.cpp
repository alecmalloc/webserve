#include "webserv.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>

int cgimain( Config config ) {
    HttpRequest req(config);

    //change to work in cgihandler with server from request
    req.setServer( config.getServerConfs()[0] );
    req.setMethod( "POST" );
    req.setUri( "./cgi-bin/test.py" );
    req.setUrl( "./cgi-bin/test.py" );
    req.setVersion( "HTTP/1.1" );
    req.setBody( "name=Mofoker" ); 

    req.setHeader( "Content-Type", "application/x-www-form-urlencoded" );
    req.setHeader( "Host", "localhost:8080" );
    req.setHeader( "User-Agent", "TestClient/1.0" );

    int result = handleCgi(req);

    return result;
}

