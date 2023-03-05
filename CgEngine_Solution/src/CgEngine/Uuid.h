#pragma once

#include <random>

namespace CgEngine {

    class Uuid {
    public:
        Uuid();

        uint32_t getUuid() const;

    private:
        uint32_t uuid;

        static std::random_device randomDevice;
        static std::mt19937_64 eng;
        static std::uniform_int_distribution<uint64_t> uniformDistribution;
    };

}
