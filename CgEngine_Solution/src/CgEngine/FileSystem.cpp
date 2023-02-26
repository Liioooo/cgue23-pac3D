#include "FileSystem.h"
#include <fstream>
#include "Logging.h"

namespace CgEngine {
    std::string FileSystem::readFileToString(const std::string &path) {
        std::ifstream file("assets/" + path);

        if (file.is_open()) {
            std::stringstream stringStream;
            stringStream << file.rdbuf();
            std::string out = stringStream.str();
            file.close();
            return out;
        }

        CG_LOGGING_ERROR("Unable to load File: {0}", "assets/" + path);
        return std::string{};
    }

    bool FileSystem::checkFileExists(const std::string &path) {
        std::ifstream file("assets/" + path);
        if (file.is_open()) {
            file.close();
            return true;
        }
        return false;
    }
}
