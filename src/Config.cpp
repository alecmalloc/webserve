#include "../inc/Config.hpp"
#include "../inc/StrUtils.hpp"

Config::Config(const std::string& filename): _config_filename(filename) {
    parse();
}

static std::pair<std::string, std::vector<std::string> > extractDirectives(const std::string& line) {
    std::istringstream iss(line);
    std::string directive;
    std::vector<std::string> subdirectives;
    std::string subdirective;

    iss >> directive;
    while (iss >> subdirective)
        subdirectives.push_back(subdirective);

    return std::make_pair(directive, subdirectives);
}

void Config::loadConfig(void) {
    std::string line;
    std::stack<std::string> contextStack;
    std::stack<std::string> directives;

    while (getline(_config_file, line)) {
        line = ft_trim(line);
        std::cout << "Processing line: " << line << '\n'; // Debug output
        if (contextStack.empty() == false)
            std::cout << "context:" << contextStack.top() << '\n';
        // scan for first server block -> server context on
        if (line.substr(0, 7) == "server ") {
            contextStack.push("server");
            std::cout << "Found server block: " << line << '\n'; // Debug output
            ServerConfig server;
            _servers.push_back(server);
        }
        // scan for location block under server -> location context on
        else if (contextStack.top() == "server" && line.substr(0, 9) == "location ") {
            contextStack.push("location");
            std::cout << "Found location block under server\n"; // Debug output
            LocationConfig location;
            _servers.back().locations.push_back(location);
        }
        // push rest of directives and pop from context
        else {
            // get rid of current context if closing brace found
            if (line.find("}") != std::string::npos)
                contextStack.pop();
            else if (line.empty())
                continue;
            // insert location directives into directives map (server -> location -> directive)
            else if (contextStack.top() == "location") {
                _servers.back().locations.back().directives.insert(extractDirectives(line));
                std::cout << "pushed directives to location block" << '\n';
            }
            // insert server directives into directives map (server -> directive)
            else if (contextStack.top() == "server") {
                _servers.back().directives.insert(extractDirectives(line));
                std::cout << "pushed directives to server block" << '\n';
            }
        }
    }
}


static bool bracesValidate(std::ifstream& file) {
    int braceCount = 0;
    std::string line;
    while (std::getline(file, line)) {
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount < 0)
                    return false;
            }
        }
    }
    if (braceCount != 0)
        return false;
    file.clear();
    file.seekg(0, std::ios::beg);
    return true;
}

void Config::parse() {
    // check if we can open file
    _config_file.open(_config_filename.c_str());
    if (!_config_file.is_open())
        throw std::runtime_error("Cannot open configuration file '" + _config_filename + "'.");
    // check that file is not empty
    if (_config_file.peek() == std::ifstream::traits_type::eof()) 
        throw std::runtime_error("Cannot open configuration file '" + _config_filename + "'.");
    // validate amt of braces
    if (bracesValidate(_config_file) == false)
        throw std::runtime_error("Misconfigured braces in '" + _config_filename + "'.");
    // parse file and load into config objects
    loadConfig();
    _config_file.close();
}