#pragma once

namespace CgEngine {

    class ShaderStorageBuffer {
    public:
        explicit ShaderStorageBuffer(uint32_t binding);
        ~ShaderStorageBuffer();

        void setData(const void* data, size_t size);

    private:
        uint32_t bufferId;

    };

}
