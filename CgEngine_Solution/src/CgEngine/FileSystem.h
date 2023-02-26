#pragma once

namespace CgEngine {

    class FileSystem {
    public:
        static std::string readFileToString(const std::string& path);
        static bool checkFileExists(const std::string& path);
    };

}
