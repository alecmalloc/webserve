#include "../inc/StrUtils.hpp"

std::string ft_trim(const std::string& str) {
    std::string s = str;

    // remove leading whitespace
    std::string::iterator it = s.begin();
    while (it != s.end() && std::isspace(*it))
        ++it;
    s.erase(s.begin(), it);

    // remove trailing whitespace
    // reverse iterator ! how sick ??
    std::string::reverse_iterator rit = s.rbegin();
    while (rit != s.rend() && std::isspace(*rit))
        ++rit;
    s.erase(rit.base(), s.end());

    return s;
}