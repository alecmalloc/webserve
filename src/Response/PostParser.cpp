#include <PostParser.hpp>


PostParser::PostParser(const std::string& boundary) : boundary(boundary) {}


std::string PostParser::urlDecode(const std::string& encoded) {
    std::string decoded;
    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%') {
            if (i + 2 < encoded.length()) {
                int value;
                std::istringstream is(encoded.substr(i + 1, 2));
                is >> std::hex >> value;
                decoded += static_cast<char>(value);
                i += 2;
            }
        }
        else if (encoded[i] == '+') {
            decoded += ' ';
        }
        else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

bool PostParser::parseUrlEncoded(const std::string& data) {
    std::istringstream stream(data);
    std::string pair;
    
    while (std::getline(stream, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != std::string::npos) {
            FormPart part;
            part.name = urlDecode(pair.substr(0, pos));
            part.content = urlDecode(pair.substr(pos + 1));
            parts.push_back(part);
        }
    }
    return true;
}

bool PostParser::parse(const std::string& data, const std::string& contentType) {
    parts.clear();
    
    if (contentType.find("multipart/form-data") != std::string::npos) {
        return parseMultipart(data);
    }
    else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
        return parseUrlEncoded(data);
    }
    
    return false;
}


bool PostParser::parseMultipart(const std::string& data){
	FormPart currentPart;
    std::string line;
    std::istringstream stream(data);

    while (std::getline(stream, line)) {
        if (line.find(boundary) != std::string::npos) {
            if (!currentPart.name.empty()) {
                parts.push_back(currentPart);
                currentPart = FormPart();
            }
            continue;
        }

        // Parse headers
        if (line.find("Content-Disposition") != std::string::npos) {
            // Extract name and filename
            size_t namePos = line.find("name=\"");
            if (namePos != std::string::npos) {
                namePos += 6;
                size_t endPos = line.find("\"", namePos);
                currentPart.name = line.substr(namePos, endPos - namePos);
            }
            
            size_t filenamePos = line.find("filename=\"");
            if (filenamePos != std::string::npos) {
                filenamePos += 10;
                size_t endPos = line.find("\"", filenamePos);
                currentPart.filename = line.substr(filenamePos, endPos - filenamePos);
            }
        }
        
        // Parse content
        if (line.empty() && !currentPart.name.empty()) {
            std::string content;
            while (std::getline(stream, line) && 
                   line.find(boundary) == std::string::npos) {
                content += line + "\n";
            }
            currentPart.content = content;
        }
    }
    return true;
}