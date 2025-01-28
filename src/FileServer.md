#include "webserv.hpp"

Response serveDirectory(const PathInfo& path) {
    Response response;

    // check if path is a directory
    if (!path.isDirectory())
        throw std::runtime_error("Path is not a directory");

    // check if any default files exist in directory
    struct stat statbuf;
    //  make c style str arr
    const char* defaultFilesArray[] = DEFAULT_FILES;
    // construct str vector using contructor that takes array, beginning of array and end of array
    std::vector<std::string> defaultFiles(defaultFilesArray, defaultFilesArray + sizeof(defaultFilesArray) / sizeof(defaultFilesArray[0]));
    for (std::vector<std::string>::const_iterator it = defaultFiles.begin(); it != defaultFiles.end(); ++it) {
        std::string fullPath = path.getFullPath() + "/" + (*it);
        if (stat(it->c_str(), &statbuf) == 0) {
            return serveFile(PathInfo(fullPath));
        }
    }

    // TODO check if directory listing is enabled ask @ moritz

    // TODO maybe move this part into validatePath so that we can error handle properly
    DIR* dir = opendir(path.getFullPath().c_str());
    if (dir == NULL) {
        throw std::runtime_error("Failed to open directory");
    }

    std::vector<std::string> directoryEntries;
    // generate directory listing
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // process each directory entry
        std::string entryName = entry->d_name;
        // skip "." and ".." entries
        if (entryName == "." || entryName == "..")
            continue;
        // add entry to response
        directoryEntries.push_back(entryName);
    }
    closedir(dir);

    // TODO convert entries to html
        // get template from ./html/templates/directoryListing.html
        // fill in placeholders and store in a new html file ./html/static
        // serveFile
    // read the HTML template
    std::ifstream templateFile("./html/templates/directoryListing.html");
    if (!templateFile.is_open()) {
        throw std::runtime_error("Failed to open template file");
    }

    // template file into string stream buffer
    std::stringstream buffer;
    buffer << templateFile.rdbuf();
    std::string htmlTemplate = buffer.str();
    templateFile.close();

    // replace {{directory_path}} with the actual directory path
    std::string directoryPath = path.getFullPath();
    size_t pos = htmlTemplate.find("{{directory_path}}");
    if (pos != std::string::npos) {
        htmlTemplate.replace(pos, 17, directoryPath);
    }

    return response;
}

Response serveFile(const PathInfo& path) {
    Response response;

    if (!path.isFile())
        throw std::runtime_error("Path is not a file");

    return response;
}