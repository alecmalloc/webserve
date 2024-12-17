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
        std::string path;
        std::map<std::string, std::string> directives;
};

// server block obj config
class ServerConfig {
    public:
        std::map<std::string, std::string> directives;
        std::vector<LocationConfig> locations;
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
