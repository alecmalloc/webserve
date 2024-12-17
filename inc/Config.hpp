#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

// nginx config files are divided into server blocks and location blocks
// server contains: directives, location blocks
// location contains: directives and paths

// location block obj config
class LocationConfig {
	public:
		//constructors
		LocationConfig( void );
		LocationConfig( const LocationConfig& og );
		~LocationConfig( void );

		//operator overloads
		LocationConfig&	operator =( const LocationConfig& og );

		//getter functions

		//setter functions
		
		//member functions


	private: 
};

// server block obj config
class ServerConfig {
	public:
		//constructors
		ServerConfig( void );
		ServeerConfig( const ServerConfig& og );
		~ServerConfig( void );

		//operator overloads
		ServerConfig&	operator =( const ServerConfig& og );

		//getter functions

		//setter functions
		
		//member functions


	private: 
		//store server names
		std::vector< std::string >	_serverNames;
		//store location diretives
		std::vector< LocationConfig >	_locations;

};

// main Config obj
class Config {
    private:
        std::string                 _config_filename;
        std::ifstream               _config_file;
        std::vector<ServerConfig>   servers;
    public:
        Config() {};
        Config(const std::string& filename);
        ~Config() {};

        void parse();
        void loadConfig(void);
};

#endif
