#include "../inc/Config.hpp"
#include "../inc/StrUtils.hpp"

Config::Config(const std::string& filename): _config_filename(filename) {
    parse();
}

void Config::loadConfig(void) {
    std::string line;
    std::stack<std::string> contextStack;

    while (getline(_config_file, line)) {
        std::cout << ft_trim(line) << '\n';
    }
}

void Config::parse() {
    // check if we can open file
    _config_file.open(_config_filename.c_str());
    if (!_config_file.is_open())
        throw std::runtime_error("Cannot open configuration file '" + _config_filename + "'.");
    // check that file is not empty
    if (_config_file.peek() == std::ifstream::traits_type::eof()) 
        throw std::runtime_error("Cannot open configuration file '" + _config_filename + "'.");
    // parse file and load into config objects
    loadConfig();
    _config_file.close();
}