#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>

class ConfigParser {
    private:
        // rest of tings
    public:
        void parse(std::string& filename);
        bool validateConfig();
};


#endif