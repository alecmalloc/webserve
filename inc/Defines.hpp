#ifndef DEFINES_HPP
# define DEFINES_HPP

//define defaults
#define DEFAULT_CONF		"conf/default.conf"
#define DEFAULT_IP		"0.0.0.0"
#define DEFAULT_PORT		8080
#define DEFAULT_BODY_SIZE	1000000
#define DEFAULT_UP_DIR		"./uploads/"

//defines for parser. Methodes Names how config stuff is called etc.
#define ALLOWED_METHODES	{ "POST", "DELETE", "GET" }
#define ALLOWED_REDIRECTS	{ "301", "302", "307", "308", "300", "304", "303" }
#define METHODE			"allowed_methods"
#define REDIRECT		"allowed_redirects"
#define ROOT			"root"
#define AINDEX			"autoindex"
#define INDEX			"index"
#define CGIPATH			"cgi_path"
#define CGIEXT			"cgi_ext"
#define UPDIR			"upload_dir"
#define LISTEN			"listen"
#define ERROR			"error_page"
#define SERVER			"server_name"
#define CLIENT			"client_max_body_size"
#define CHUNK_ENC		"use_chunked_encoding"
#define CHUNK_SIZE		"chunk_size"


//defines for epoll
#define	MAX_EVENTS 	10000
#define BUFFERSIZE	10000
#define TIMEOUT_TIME	2000
#define MAX_BYTES	5000000

//color codes for output
# define RED		"\x1B[31m"
# define GREEN		"\x1B[32m"
# define YELLOW		"\x1B[33m"
# define ORANGE		"\x1B[34m"
# define PURPLE		"\x1B[35m"
# define BLUE		"\x1B[36m"
# define BOLD		"\x1B[1m"
# define GRAY		"\x1B[90m"
# define END		"\x1B[0m"

// defines for file operations
#define DEFAULT_FILES       { "index.html","index.htm","default.html", "default.htm", "home.html", "home.htm" }

#endif
