#include "webserv.hpp" 

void    Response::handleRedirectRequest( void ){

	int             code;
	std::string     dest;

	//get map of location redirects 
	std::map<std::string, std::string > locationRedirects = _locationConf.getAllowedRedirects();

	// get map of server redirects
	std::map<std::string, std::string > serverRedirects = _serverConf.getAllowedRedirects();

	// ternary operator evaluate which one to pass to handle redirect
	std::map<std::string, std::string > redirect = locationRedirects.empty() \
		? serverRedirects : locationRedirects;

	// iterator for redirect access
	std::map<std::string, std::string >::iterator it;

	it = redirect.begin();

	// convert str to int for red code
	std::stringstream ss(it->first);
	ss >> code;

	// set dest address
	// we skip first character becuase moritz puts in a / at the beginning
	dest = it->second.substr(1, it->second.length());

	// set req code to the first string (301, 302) of redirect vec
	setStatus( code );

	// set _redirectDest to dest. this gets returned as Location in headers
	setRedirectDest( dest );

	// empty body on redirects
	setBody("");
}
