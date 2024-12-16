#ifndef CONFIG_HPP
#define CONFIGP_HPP

#include <string>
#include <map>

class LocationConfig {
    public:
        std::string path;
        std::map<std::string, std::string> directives;
};

class ServerConfig {
    public:
        std::map<std::string, std::string> directives;
        std::vector<LocationConfig> locations;
};

class Config {
    private:
        std::string config_filename;
        std::vector<ServerConfig> servers;
    public:
        void parse(std::string& filename);
        bool validateConfig();
};


#endif