#include "../inc/HttpHelpers.hpp"

#include <cctype>
#include <sstream>

std::string urlDecoder(const std::string& encodedUrl) {
    std::string decodedUrl;

    for (size_t i = 0; i < encodedUrl.length(); ++i) {
        if (encodedUrl[i] == '%' && i + 2 < encodedUrl.length()) {
            int value;
            std::istringstream is(encodedUrl.substr(i + 1, 2));
            if (is >> std::hex >> value)
                decodedUrl += static_cast<char>(value);
            i += 2;
        }
        else if (encodedUrl[i] == '+')
            decodedUrl += ' ';
        else
            decodedUrl += encodedUrl[i];
    }

    return decodedUrl;
}