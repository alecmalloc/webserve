#ifndef DEFINES_HPP
# define DEFINES_HPP

#define DEFAULT_CONF		"conf/default.conf"
#define DEFAULT_IP		"0.0.0.0"
#define DEFAULT_PORT		"80"
#define MAXBODYSIZE		10000
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

#endif
