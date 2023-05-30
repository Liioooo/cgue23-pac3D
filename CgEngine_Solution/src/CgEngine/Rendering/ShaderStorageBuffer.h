#pragma once

namespace CgEngine {

    class ShaderStorageBuffer {
    public:
        explicit ShaderStorageBuffer();
        ~ShaderStorageBuffer();

        void setData(const void* data, size_t size);
        void bind(uint32_t binding) const;

    private:
        uint32_t bufferId;

    };

}
