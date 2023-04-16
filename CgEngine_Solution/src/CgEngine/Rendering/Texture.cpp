#include "Texture.h"
#include "Asserts.h"
#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

namespace CgEngine {

    namespace TextureUtils {
        int getOpenGLTextureFormat(TextureFormat format) {
            switch (format) {
                case TextureFormat::RGB:     return GL_RGB;
                case TextureFormat::RGBA:    return GL_RGBA;
                case TextureFormat::Float16: return GL_RGBA16F;
                case TextureFormat::Float32: return GL_RGBA32F;
                case TextureFormat::Depth:   return GL_DEPTH_COMPONENT32F;
            }
            return 0;
        }

        int getTextureFormatType(TextureFormat format) {
            if (format == TextureFormat::Float16 || format == TextureFormat::Float32 || format == TextureFormat::Depth) {
                return GL_FLOAT;
            }
            return GL_UNSIGNED_BYTE;
        }

        int getTextureWrap(TextureWrap wrap) {
            switch (wrap) {
                case TextureWrap::Repeat:       return GL_REPEAT;
                case TextureWrap::Clamp:        return GL_CLAMP_TO_EDGE;
                case TextureWrap::ClampBorder:  return GL_CLAMP_TO_BORDER;
            }
            return 0;
        }

        void setClampBorderColor(const glm::vec4& color, unsigned int textureType) {
            glTexParameterfv(textureType, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        }

        void applyMipMapFiltering(MipMapFiltering mipMapFiltering, unsigned int textureType) {
            switch (mipMapFiltering) {
                case MipMapFiltering::Nearest: {
                    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    return;
                }
                case MipMapFiltering::Bilinear: {
                    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    return;
                }
                case MipMapFiltering::Trilinear: {
                    glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    return;
                }
            }
        }

        uint32_t calculateMipCount(uint32_t width, uint32_t height) {
            return static_cast<uint32_t>(std::floor(std::log2(glm::min(width, height))) + 1);
        }
    }

    Texture2D *Texture2D::createResource(const std::string &name) {
        return new Texture2D(name, true);
    }

    Texture2D::Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, MipMapFiltering mipMapFiltering) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D);
        GLint textureWrap = TextureUtils::getTextureWrap(wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

        GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
        GLenum type = TextureUtils::getTextureFormatType(format);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format == TextureFormat::Depth ? GL_DEPTH_COMPONENT : GL_RGBA, type, nullptr);
    }

    Texture2D::Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, const void* data, MipMapFiltering mipMapFiltering) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D);
        GLint textureWrap = TextureUtils::getTextureWrap(wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

        GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
        GLenum type = TextureUtils::getTextureFormatType(format);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format == TextureFormat::Depth ? GL_DEPTH_COMPONENT : GL_RGBA, type, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        loaded = true;
    }

    Texture2D::Texture2D(const std::string &path, bool srgb, TextureWrap wrap, MipMapFiltering mipMapFiltering) {
        int loadWidth, loadHeight, channels;

        unsigned char* data;

        if (stbi_is_hdr(path.c_str())) {
            data = (unsigned char*)(stbi_loadf(path.c_str(), &loadWidth, &loadHeight, &channels, 0));
            format = TextureFormat::Float32;
        } else {
            data = stbi_load(path.c_str(), &loadWidth, &loadHeight, &channels, STBI_rgb);
            format = TextureFormat::RGB;
        }

        if (data) {
            loaded = true;
        } else {
            return;
        }

        width = loadWidth;
        height = loadHeight;

        if (srgb) {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D);
            GLint textureWrap = TextureUtils::getTextureWrap(wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

            GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
            GLenum type = TextureUtils::getTextureFormatType(format);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, internalFormat, type, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D);
            GLint textureWrap = TextureUtils::getTextureWrap(wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

            GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
            GLenum type = TextureUtils::getTextureFormatType(format);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, type, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        }

        stbi_image_free(data);
    }

    Texture2D::~Texture2D() {
        glDeleteTextures(1, &id);
    }

    uint32_t Texture2D::getWidth() const {
        return width;
    }

    uint32_t Texture2D::getHeight() const {
        return height;
    }

    uint32_t Texture2D::getWidthForMip(uint32_t mip) const {
        int w;
        glBindTexture(GL_TEXTURE_2D, id);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mip, GL_TEXTURE_WIDTH, &w);
        return w;
    }

    uint32_t Texture2D::getHeightForMip(uint32_t mip) const {
        int h;
        glBindTexture(GL_TEXTURE_2D, id);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mip, GL_TEXTURE_HEIGHT, &h);
        return h;
    }

    TextureFormat Texture2D::getFormat() const {
        return format;
    }

    uint32_t Texture2D::getRendererId() const {
        return id;
    }

    void Texture2D::bind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void Texture2D::generateMipMaps() {
        glBindTexture(GL_TEXTURE_2D, id);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Texture2D::setClampBorderColor(const glm::vec4& color) {
        glBindTexture(GL_TEXTURE_2D, id);
        TextureUtils::setClampBorderColor(color, GL_TEXTURE_2D);
    }

    bool Texture2D::isLoaded() const {
        return loaded;
    }

    Texture2DArray::Texture2DArray(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, uint32_t count, MipMapFiltering mipMapFiltering) : format(format), width(width), height(height), count(count) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);

        TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D_ARRAY);
        GLint textureWrap = TextureUtils::getTextureWrap(wrap);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, textureWrap);

        GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
        GLenum type = TextureUtils::getTextureFormatType(format);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, width, height, count, 0, format == TextureFormat::Depth ? GL_DEPTH_COMPONENT : GL_RGBA, type, nullptr);
    }

    Texture2DArray::~Texture2DArray() {
        glDeleteTextures(1, &id);
    }

    uint32_t Texture2DArray::getWidth() const {
        return width;
    }

    uint32_t Texture2DArray::getHeight() const {
        return height;
    }

    uint32_t Texture2DArray::getCount() const {
        return count;
    }

    TextureFormat Texture2DArray::getFormat() const {
        return format;
    }

    uint32_t Texture2DArray::getRendererId() const {
        return id;
    }

    void Texture2DArray::bind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
    }

    void Texture2DArray::generateMipMaps() {
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }

    void Texture2DArray::setClampBorderColor(const glm::vec4& color) {
        glBindTexture(GL_TEXTURE_2D_ARRAY, id);
        TextureUtils::setClampBorderColor(color, GL_TEXTURE_2D_ARRAY);
    }

    TextureCube* TextureCube::createResource(const std::string& name) {
        return new TextureCube(TextureFormat::RGB, 1, 1);
    }

    TextureCube::TextureCube(TextureFormat format, uint32_t width, uint32_t height, MipMapFiltering mipMapFiltering) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
        GLenum type = TextureUtils::getTextureFormatType(format);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, width, height, 0, GL_RGB, type, nullptr);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, width, height, 0, GL_RGB, type, nullptr);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, width, height, 0, GL_RGB, type, nullptr);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, width, height, 0, GL_RGB, type, nullptr);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, width, height, 0, GL_RGB, type, nullptr);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, width, height, 0, GL_RGB, type, nullptr);

        TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_CUBE_MAP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    TextureCube::TextureCube(TextureFormat format, uint32_t width, uint32_t height, const void* data, MipMapFiltering mipMapFiltering) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_CUBE_MAP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
        GLenum type = TextureUtils::getTextureFormatType(format);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, width, height, 0, GL_RGBA, type, data);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, width, height, 0, GL_RGBA, type, data);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, width, height, 0, GL_RGBA, type, data);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, width, height, 0, GL_RGBA, type, data);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, width, height, 0, GL_RGBA, type, data);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, width, height, 0, GL_RGBA, type, data);
    }

    TextureCube::~TextureCube() {
        glDeleteTextures(1, &id);
    }

    uint32_t TextureCube::getWidth() const {
        return width;
    }

    uint32_t TextureCube::getHeight() const {
        return height;
    }

    TextureFormat TextureCube::getFormat() const {
        return format;
    }

    uint32_t TextureCube::getRendererId() const {
        return id;
    }

    void TextureCube::bind(uint32_t slot) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    }

    void TextureCube::generateMipMaps() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
}
