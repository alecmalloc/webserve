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

//cpp header
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>

//webserv header
#include "Location.hpp"
#include "Config.hpp"
#include "Defines.hpp"

#define	MAX_EVENTS 	100
# define RED		"\x1B[31m"
# define GREEN		"\x1B[32m"
# define YELLOW		"\x1B[33m"
# define ORANGE		"\x1B[34m"
# define PURPLE		"\x1B[35m"
# define BLUE		"\x1B[36m"
# define BOLD		"\x1B[1m"
# define GRAY		"\x1B[90m"
# define END		"\x1B[0m"

void	runServer( void );

#endif
