#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//sys header
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//c header
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>
#include <dirent.h>

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
#include <algorithm>
#include <stdexcept>

//webserv header
#include "Location.hpp"
#include "Server.hpp"
#include "ServerConf.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "Defines.hpp"
#include "HttpRequest.hpp"
#include "StrUtils.hpp"
#include "PathInfo.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "Cgi.hpp"

void	runServer( Config& conf );

#endif
