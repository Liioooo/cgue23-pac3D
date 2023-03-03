#include "Texture.h"
#include "Asserts.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

namespace CgEngine {
    Texture2D *Texture2D::createResource(const std::string &name) {
        return new Texture2D(name, true);
    }

    Texture2D::Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, MipMapFiltering mipMapFiltering) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D);
        GLint textureWrap = wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

        GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
        GLenum type = TextureUtils::getTextureFormatType(format);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, type, nullptr);
    }

    Texture2D::Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, const void* data, MipMapFiltering mipMapFiltering) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D);
        GLint textureWrap = wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

        GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
        GLenum type = TextureUtils::getTextureFormatType(format);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, type, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        loaded = true;
    }

    Texture2D::Texture2D(const std::string &path, bool srgb, MipMapFiltering mipMapFiltering) {
        int loadWidth, loadHeight, channels;

        unsigned char* data;

        if (stbi_is_hdr(path.c_str())) {
            data = (unsigned char*)(stbi_loadf(path.c_str(), &loadWidth, &loadHeight, &channels, 0));
            format = TextureFormat::Float32;
        } else {
            data = stbi_load(path.c_str(), &loadWidth, &loadHeight, &channels, STBI_rgb_alpha);
            format = TextureFormat::RGBA;
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
            GLenum type = TextureUtils::getTextureFormatType(format);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, internalFormat, type, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            TextureUtils::applyMipMapFiltering(mipMapFiltering, GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            GLint internalFormat = TextureUtils::getOpenGLTextureFormat(format);
            GLenum type = TextureUtils::getTextureFormatType(format);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, type, data);

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

    bool Texture2D::isLoaded() const {
        return loaded;
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
