#pragma once

namespace CgEngine {

    class Shader {
    public:
        static Shader* createResource(const std::string& name);

        explicit Shader(std::string name);
        ~Shader();

        void bind();
        void unbind();

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

        std::string loadShaderSourceCode(const std::string& type);
        void checkErrors(uint32_t id, const std::string &type) const;
        int32_t getUniformLocation(const std::string& name);

    };

}
