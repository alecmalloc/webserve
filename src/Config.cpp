#include "../inc/Config.hpp"

Config::Config(const std::string& filename): _config_file(filename) {
    parse();
}

static bool bracesValidate(std::ifstream& file) {
    std::stack<char> braceStack;
    std::string line;
    while (std::getline(file, line)) {
        for (size_t i = 0; i < line.length(); ++i) {
                char c = line[i];
                if (c == '{') {
                    braceStack.push('{');
                } else if (c == '}') {
                    // missing closing brace
                    if (braceStack.empty())
                        return false;
                    braceStack.pop();
                }
        }
    }
    // missing first brace
    if (!braceStack.empty())
        return false;
    // Reset file pointer to the beginning
    file.clear();
    file.seekg(0, std::ios::beg);
    return true;
}

void Config::parse() {
    // check if we can open file
    std::ifstream file(_config_file);
    if (!file.is_open())
        throw std::runtime_error("Cannot open configuration file '" + _config_file + "'.");
    // check that file is not empty
    if (file.peek() == std::ifstream::traits_type::eof()) 
        throw std::runtime_error("Cannot open configuration file '" + _config_file + "'.");
    if (bracesValidate(file) == false)
        throw std::runtime_error("Missing or wrong braces in'" + _config_file + "'." );


}

bool Config::validateConfig() {
    return true;
}