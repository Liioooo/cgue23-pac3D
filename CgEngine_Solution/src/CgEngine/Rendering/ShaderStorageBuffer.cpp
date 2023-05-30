#include "ShaderStorageBuffer.h"
#include "glad/glad.h"

namespace CgEngine {
    ShaderStorageBuffer::ShaderStorageBuffer() {
        glCreateBuffers(1, &bufferId);
    }

    ShaderStorageBuffer::~ShaderStorageBuffer() {
        glDeleteBuffers(1, &bufferId);
    }

    void ShaderStorageBuffer::setData(const void* data, size_t size) {
        glNamedBufferData(bufferId, size, data, GL_DYNAMIC_DRAW);
    }

    void ShaderStorageBuffer::bind(uint32_t binding) const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, bufferId);
    }

}
