#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <string>

#define	MAX_EVENTS 	100
#define RED 		"\033[0;31m"
#define GREEN 		"\033[0;32m"
#define BLUE		"\033[0;34m"
#define RESET		"\033[0;0m"

void	runServer( void );

#endif
