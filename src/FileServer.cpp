#include "../inc/FileServer.hpp"


Response serveDirectory(const PathInfo& path) {
    Response response;

    // check if path is a directory
    if (!path.isDirectory())
        throw std::runtime_error("Path is not a directory");

    // check if any default files exist in directory
    struct stat statbuf;
    for (std::vector<std::string>::const_iterator it = defaultFiles.begin(); it != defaultFiles.end(); ++it) {
        std::string fullPath = path.getFullPath() + "/" + (*it);
        if (stat(it->c_str(), &statbuf) == 0) {
            return serveFile(PathInfo(fullPath));
        }
    }

    // TODO check if directory listing is enabled ask @ moritz

    // TODO maybe move this part into validatePath so that we can error handle properly
    DIR* dir = opendir(path.getFullPath().c_str());
    if (dir == nullptr) {
        throw std::runtime_error("Failed to open directory");
    }

    std::vector<std::string> directoryEntries;
    // generate directory listing
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
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
    return response;
}


Response serveFile(const PathInfo& path) {
    Response response;

    if (!path.isFile())
        throw std::runtime_error("Path is not a file");

}