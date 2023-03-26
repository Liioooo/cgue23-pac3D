#include "Shader.h"
#include "Texture.h"
#include "FileSystem.h"
#include "Logging.h"
#include "Asserts.h"
#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.h"

namespace CgEngine {

    namespace ShaderUtils {
        static std::string loadShaderSourceCode(const std::string& name, const std::string& type) {
            if (!FileSystem::checkFileExists(FileSystem::getAsEnginePath("shaders/" + name + "_" + type + ".glsl"))) {
                return "";
            }
            return FileSystem::readFileToString(FileSystem::getAsEnginePath("shaders/" + name + "_" + type + ".glsl"));
        }

        static void checkErrors(uint32_t id, const std::string &type) {
            int isCompiled;
            int maxLength;

            if (type == "PROGRAM") {
                glGetProgramiv(id, GL_LINK_STATUS, &isCompiled);
                if (!isCompiled) {
                    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);
                    char* infoLog = new char[maxLength];
                    glGetProgramInfoLog(id, maxLength, &maxLength, infoLog);
                    CG_LOGGING_ERROR(type + " ERROR: " + infoLog);
                }
            } else {
                glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
                if (!isCompiled) {
                    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
                    char* infoLog = new char[maxLength];
                    glGetShaderInfoLog(id, maxLength, &maxLength, infoLog);
                    CG_LOGGING_ERROR(type + " ERROR: " + infoLog);
                }
            }
        }

        static int32_t getUniformLocation(uint32_t programId, std::unordered_map<std::string, int32_t>& uniformLocations, const std::string& name) {
            if (uniformLocations.count(name) == 0) {
                int32_t location = glGetUniformLocation(programId, name.c_str());

                CG_ASSERT(location != -1, "Uniform doesn't exist! Uniform:" + name);

                uniformLocations.insert({name, location});
                return location;
            }
            return uniformLocations[name];
        }

        int getSizeForShaderDataType(ShaderDataType type) {
            switch (type) {
                case ShaderDataType::Float:    return 4;
                case ShaderDataType::Float2:   return 4 * 2;
                case ShaderDataType::Float3:   return 4 * 3;
                case ShaderDataType::Float4:   return 4 * 4;
                case ShaderDataType::Mat3:     return 4 * 3 * 3;
                case ShaderDataType::Mat4:     return 4 * 4 * 4;
                case ShaderDataType::Int:      return 4;
                case ShaderDataType::Int2:     return 4 * 2;
                case ShaderDataType::Int3:     return 4 * 3;
                case ShaderDataType::Int4:     return 4 * 4;
                case ShaderDataType::Bool:     return 1;
            }
            return 0;
        }

        unsigned int shaderDataTypeToOpenGLBaseType(ShaderDataType type) {
            switch (type) {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4: {
                    return GL_FLOAT;
                }
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4: {
                    return GL_INT;
                }
                case ShaderDataType::Bool: {
                    return GL_BOOL;
                }
            }
            return 0;
        }
    }

    Shader* Shader::createResource(const std::string& name) {
        return new Shader(name);
    }

    Shader::Shader(std::string name) : name(std::move(name)) {
        std::string vertexSource = ShaderUtils::loadShaderSourceCode(this->name, "vertex");
        std::string fragmentSource = ShaderUtils::loadShaderSourceCode(this->name, "fragment");
        std::string geometrySource = ShaderUtils::loadShaderSourceCode(this->name, "geometry");

        programId = glCreateProgram();

        if (!vertexSource.empty()) {
            const char* cString = vertexSource.c_str();
            uint32_t id = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(id, 1, &cString, nullptr);
            glCompileShader(id);
            ShaderUtils::checkErrors(id, "VERTEX");
            glAttachShader(programId, id);
        }
        if (!fragmentSource.empty()) {
            const char* cString = fragmentSource.c_str();
            uint32_t id = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(id, 1, &cString, nullptr);
            glCompileShader(id);
            ShaderUtils::checkErrors(id, "FRAGMENT");
            glAttachShader(programId, id);
        }
        if (!geometrySource.empty()) {
            const char* cString = geometrySource.c_str();
            uint32_t id = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(id, 1, &cString, nullptr);
            glCompileShader(id);
            ShaderUtils::checkErrors(id, "GEOMETRY");
            glAttachShader(programId, id);
        }

        glLinkProgram(programId);
        ShaderUtils::checkErrors(programId, "PROGRAM");
    }

    Shader::~Shader() {
        glDeleteProgram(programId);
    }

    void Shader::bind() {
        glUseProgram(programId);
    }

    uint32_t Shader::getProgramId() {
        return programId;
    }

    void Shader::setBool(const std::string &name, bool value) {
        glUniform1i(ShaderUtils::getUniformLocation(programId, uniformLocations, name), value);
    }

    void Shader::setInt(const std::string &name, int value) {
        glUniform1i(ShaderUtils::getUniformLocation(programId, uniformLocations, name), value);
    }

    void Shader::setFloat(const std::string &name, float value) {
        glUniform1f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), value);
    }

    void Shader::setVec2(const std::string &name, const glm::vec2 &vec) {
        glUniform2f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), vec.x, vec.y);
    }

    void Shader::setVec3(const std::string &name, const glm::vec3 &vec) {
        glUniform3f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), vec.x, vec.y, vec.z);
    }

    void Shader::setVec4(const std::string &name, const glm::vec4 &vec) {
        glUniform4f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), vec.x, vec.y, vec.z, vec.w);
    }

    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) {
        glUniformMatrix3fv(ShaderUtils::getUniformLocation(programId, uniformLocations, name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) {
        glUniformMatrix4fv(ShaderUtils::getUniformLocation(programId, uniformLocations, name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::setTexture2D(uint32_t textureRendererId, uint32_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureRendererId);
    }

    void Shader::setTexture2DArray(uint32_t textureRendererId, uint32_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureRendererId);
    }

    void Shader::setTextureCube(uint32_t textureRendererId, uint32_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureRendererId);
    }

    ComputeShader *ComputeShader::createResource(const std::string &name) {
        return new ComputeShader(name);
    }

    ComputeShader::ComputeShader(std::string name) : name(std::move(name)) {
        std::string source = ShaderUtils::loadShaderSourceCode(this->name, "comp");

        programId = glCreateProgram();

        const char* cString = source.c_str();
        uint32_t shaderId = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shaderId, 1, &cString, nullptr);
        glCompileShader(shaderId);
        ShaderUtils::checkErrors(shaderId, "COMPUTE");
        glAttachShader(programId, shaderId);

        glLinkProgram(programId);
        ShaderUtils::checkErrors(programId, "PROGRAM");
    }

    ComputeShader::~ComputeShader() {
        glDeleteProgram(programId);
    }

    void ComputeShader::bind() {
        glUseProgram(programId);
    }

    uint32_t ComputeShader::getProgramId() {
        return programId;
    }

    void ComputeShader::dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) {
        glDispatchCompute(groupsX, groupsY, groupsZ);
    }

    void ComputeShader::waitForMemoryBarrier() {
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }

    void ComputeShader::setBool(const std::string &name, bool value) {
        glUniform1i(ShaderUtils::getUniformLocation(programId, uniformLocations, name), value);
    }

    void ComputeShader::setInt(const std::string &name, int value) {
        glUniform1i(ShaderUtils::getUniformLocation(programId, uniformLocations, name), value);
    }

    void ComputeShader::setFloat(const std::string &name, float value) {
        glUniform1f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), value);
    }

    void ComputeShader::setVec2(const std::string &name, const glm::vec2 &vec) {
        glUniform2f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), vec.x, vec.y);
    }

    void ComputeShader::setVec3(const std::string &name, const glm::vec3 &vec) {
        glUniform3f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), vec.x, vec.y, vec.z);
    }

    void ComputeShader::setVec4(const std::string &name, const glm::vec4 &vec) {
        glUniform4f(ShaderUtils::getUniformLocation(programId, uniformLocations, name), vec.x, vec.y, vec.z, vec.w);
    }

    void ComputeShader::setMat3(const std::string &name, const glm::mat3 &mat) {
        glUniformMatrix3fv(ShaderUtils::getUniformLocation(programId, uniformLocations, name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void ComputeShader::setMat4(const std::string &name, const glm::mat4 &mat) {
        glUniformMatrix4fv(ShaderUtils::getUniformLocation(programId, uniformLocations, name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void ComputeShader::setTexture2D(Texture2D& texture2D, uint32_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture2D.getRendererId());
    }

    void ComputeShader::setImage2D(Texture2D &texture, uint32_t textureUnit, ShaderStorageAccess storageAccess, uint32_t level) {
        glBindImageTexture(textureUnit, texture.getRendererId(), level, GL_FALSE, 0, static_cast<GLuint>(storageAccess), TextureUtils::getOpenGLTextureFormat(texture.getFormat()));
    }

    void ComputeShader::setTextureCube(TextureCube &texture, uint32_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.getRendererId());
    }

    void ComputeShader::setImageCube(TextureCube& texture, uint32_t textureUnit, ShaderStorageAccess storageAccess, uint32_t level) {
        glBindImageTexture(textureUnit, texture.getRendererId(), level, GL_TRUE, 0, static_cast<GLuint>(storageAccess), TextureUtils::getOpenGLTextureFormat(texture.getFormat()));
    }
}
