#include "Shader.h"
#include "FileSystem.h"
#include "glad/glad.h"
#include "Logging.h"
#include "Asserts.h"
#include <glm/gtc/type_ptr.hpp>

namespace CgEngine {
    Shader* Shader::createResource(const std::string& name) {
        return new Shader(name);
    }

    Shader::Shader(std::string name) : name(std::move(name)) {
        std::string vertexSource = loadShaderSourceCode("vertex");
        std::string fragmentSource = loadShaderSourceCode("fragment");
        std::string geometrySource = loadShaderSourceCode("geometry");

        programId = glCreateProgram();

        if (!vertexSource.empty()) {
            const char* cString = vertexSource.c_str();
            uint32_t id = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(id, 1, &cString, nullptr);
            glCompileShader(id);
            checkErrors(id, "VERTEX");
            glAttachShader(programId, id);
        }
        if (!fragmentSource.empty()) {
            const char* cString = fragmentSource.c_str();
            uint32_t id = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(id, 1, &cString, nullptr);
            glCompileShader(id);
            checkErrors(id, "FRAGMENT");
            glAttachShader(programId, id);
        }
        if (!geometrySource.empty()) {
            const char* cString = geometrySource.c_str();
            uint32_t id = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(id, 1, &cString, nullptr);
            glCompileShader(id);
            checkErrors(id, "GEOMETRY");
            glAttachShader(programId, id);
        }

        glLinkProgram(programId);
        checkErrors(programId, "PROGRAM");
    }

    Shader::~Shader() {
        glDeleteProgram(programId);
    }

    void Shader::bind() {
        glUseProgram(programId);
    }

    void Shader::unbind() {
        glUseProgram(0);
    }

    uint32_t Shader::getProgramId() {
        return programId;
    }

    void Shader::setBool(const std::string &name, bool value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::setInt(const std::string &name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }

    void Shader::setFloat(const std::string &name, float value) {
        glUniform1f(getUniformLocation(name), value);
    }

    void Shader::setVec2(const std::string &name, const glm::vec2 &vec) {
        glUniform2f(getUniformLocation(name), vec.x, vec.y);
    }

    void Shader::setVec3(const std::string &name, const glm::vec3 &vec) {
        glUniform3f(getUniformLocation(name), vec.x, vec.y, vec.z);
    }

    void Shader::setVec4(const std::string &name, const glm::vec4 &vec) {
        glUniform4f(getUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
    }

    void Shader::setMat3(const std::string &name, const glm::mat3 &mat) {
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::setTexture2D(uint32_t textureRendererId, uint32_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureRendererId);
    }

    void Shader::setTextureCube(uint32_t textureRendererId, uint32_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureRendererId);
    }

    std::string Shader::loadShaderSourceCode(const std::string& type) {
        if (!FileSystem::checkFileExists("shaders/" + name + "_" + type + ".glsl")) {
            return "";
        }
        return FileSystem::readFileToString("shaders/" + name + "_" + type + ".glsl");
    }

    void Shader::checkErrors(uint32_t id, const std::string &type) const {
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

    int32_t Shader::getUniformLocation(const std::string& name) {
        if (uniformLocations.count(name) == 0) {
            int32_t location = glGetUniformLocation(programId, name.c_str());

            CG_ASSERT(location != -1, "Uniform doesn't exist! Shader: " + this->name + ": " + name);

            uniformLocations.insert({name, location});
            return location;
        }
        return uniformLocations[name];
    }
}
