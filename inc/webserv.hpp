#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//sys header
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//c header
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>

//cpp header
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm> 
#include <vector>
#include <map>

//webserv header
#include "Location.hpp"
#include "Config.hpp"
#include "Defines.hpp"
#include "HttpRequest.hpp"
#include "StrUtils.hpp"
#include "Response.hpp"
#include "Server.hpp"

void	runServer( Config& conf );

#endif
