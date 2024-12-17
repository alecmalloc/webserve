#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <fstream>
#include <vector>
#include "Server.hpp"
#include "Location.hpp"

#define DEFAULT_CONF conf/default.conf

class Config {
    private:
        std::string			_config_filename;
        std::ifstream			_config_file;
        std::vector< Server >   	_servers;

    public:
	//cosntructors
        Config() {};
        Config(const std::string& filename);
        ~Config() {};

	//operator overloads
	Config&		operator =( const Config& og );

	//getters
	std::vector< Server >	getServers( void );

	//setters

	//member fucnitons
        void parse();
        void loadConfig(void);
};

#endif
