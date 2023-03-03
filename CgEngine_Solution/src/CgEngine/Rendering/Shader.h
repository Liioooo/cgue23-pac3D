#pragma once

#include "glad/glad.h"

namespace CgEngine {

    class Texture2D;
    class TextureCube;

    namespace ShaderUtils {
        static std::string loadShaderSourceCode(const std::string& name, const std::string& type);
        static void checkErrors(uint32_t id, const std::string &type);
        static int32_t getUniformLocation(uint32_t programId, std::unordered_map<std::string, int32_t>& uniformLocations, const std::string& name);
    }

    enum class ShaderStorageAccess {
        WriteOnly = GL_WRITE_ONLY,
        ReadOnly = GL_READ_ONLY,
        ReadWrite = GL_READ_WRITE
    };

    class Shader {
    public:
        static Shader* createResource(const std::string& name);

        explicit Shader(std::string name);
        ~Shader();

        void bind();

        uint32_t getProgramId();

        void setBool(const std::string& name, bool value);
        void setInt(const std::string& name, int value);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& vec);
        void setVec3(const std::string& name, const glm::vec3& vec);
        void setVec4(const std::string& name, const glm::vec4& vec);
        void setMat3(const std::string& name, const glm::mat3& mat);
        void setMat4(const std::string& name, const glm::mat4& mat);
        void setTexture2D(uint32_t textureRendererId, uint32_t textureUnit);
        void setTextureCube(uint32_t textureRendererId, uint32_t textureUnit);

    private:
        std::string name;
        uint32_t programId;
        std::unordered_map<std::string, int32_t> uniformLocations{};
    };

    class ComputeShader {
    public:
        static ComputeShader* createResource(const std::string& name);

        explicit ComputeShader(std::string name);
        ~ComputeShader();

        void bind();

        uint32_t getProgramId();

        void dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ);
        void waitForMemoryBarrier();

        void setBool(const std::string& name, bool value);
        void setInt(const std::string& name, int value);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& vec);
        void setVec3(const std::string& name, const glm::vec3& vec);
        void setVec4(const std::string& name, const glm::vec4& vec);
        void setMat3(const std::string& name, const glm::mat3& mat);
        void setMat4(const std::string& name, const glm::mat4& mat);
        void setTexture2D(Texture2D& texture, uint32_t textureUnit);
        void setImage2D(Texture2D& texture, uint32_t textureUnit, ShaderStorageAccess storageAccess, uint32_t level = 0);
        void setTextureCube(TextureCube& texture, uint32_t textureUnit);
        void setImageCube(TextureCube& texture, uint32_t textureUnit, ShaderStorageAccess storageAccess, uint32_t level = 0);

    private:
        std::string name;
        uint32_t programId;
        std::unordered_map<std::string, int32_t> uniformLocations{};
    };

}
