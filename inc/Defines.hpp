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

// defines for response
#define HTTP_VERSION    "HTTP/1.1"


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

#define CSS_GLOBAL          "<style>@import url('https://fonts.cdnfonts.com/css/jetbrains-mono-2');:root{--font-family:\"JetBrains Mono\",monospace;--line-height:1.20rem;--border-thickness:2px;--text-color:#000;--text-color-alt:#666;--background-color:#fff;--background-color-alt:#eee;--font-weight-normal:500;--font-weight-medium:600;--font-weight-bold:800}body{font-family:var(--font-family);font-weight:var(--font-weight-normal);color:var(--text-color);background-color:var(--background-color);max-width:1000px;margin:0 auto;padding:2rem}.header{border-bottom:var(--border-thickness) solid var(--text-color);margin-bottom:2rem}.section{border:var(--border-thickness) solid var(--text-color);padding:1rem;margin-bottom:1rem}.method-badge{background:var(--background-color-alt);padding:0.2rem 0.5rem;font-size:0.8rem}button,.btn{font-family:var(--font-family);background:var(--text-color);color:var(--background-color);border:none;padding:0.5rem 1rem;cursor:pointer;text-decoration:none;display:inline-block;margin:0.25rem}input,select{font-family:var(--font-family);border:var(--border-thickness) solid var(--text-color);padding:0.5rem;margin:0.5rem 0;width:50%}.form-group{margin-bottom:1rem}</style>";

#endif
