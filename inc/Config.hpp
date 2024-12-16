#ifndef CONFIG_HPP
#define CONFIGP_HPP

#include <string>
#include <map>

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
        std::string config_filename;
        std::vector<ServerConfig> servers;
    public:
        Config() {return;};
        Config(const std::string& filename);
        ~Config() {};

        void parse(std::string& filename);
        bool validateConfig();
};


#endif