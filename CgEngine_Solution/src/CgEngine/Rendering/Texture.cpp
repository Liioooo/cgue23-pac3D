#include "Texture.h"
#include "Asserts.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

namespace CgEngine {
    Texture2D *Texture2D::createResource(const std::string &name) {
        return new Texture2D("assets/" + name, true);
    }

    Texture2D::Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        GLint textureWrap = wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

        GLint internalFormat = getOpenGLTextureFormat(format);
        GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat, type, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture2D::Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, const void* data) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        GLint textureWrap = wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrap);

        GLint internalFormat = getOpenGLTextureFormat(format);
        GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat, type, data);
        glBindTexture(GL_TEXTURE_2D, 0);

        loaded = true;
    }

    Texture2D::Texture2D(const std::string &path, bool srgb) {
        int loadWidth, loadHeight, channels;

        unsigned char* data;

        if (stbi_is_hdr(path.c_str())) {
            data = (unsigned char*)(stbi_loadf(path.c_str(), &loadWidth, &loadHeight, &channels, 0));
            format = TextureFormat::Float16;
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

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            GLint internalFormat = getOpenGLTextureFormat(format);
            GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, internalFormat, type, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        } else {
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            GLint internalFormat = getOpenGLTextureFormat(format);
            GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormat, type, data);

            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
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

    bool Texture2D::isLoaded() const {
        return loaded;
    }

    TextureCube::TextureCube(TextureFormat format, uint32_t width, uint32_t height) : format(format), width(width), height(height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        uint32_t faceWidth = width / 4;
        uint32_t faceHeight = height / 3;
        CG_ASSERT(faceWidth == faceHeight, "Non-square faces!");


        GLint internalFormat = getOpenGLTextureFormat(format);
        GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, internalFormat, type, nullptr);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, internalFormat, type, nullptr);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, internalFormat, type, nullptr);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, internalFormat, type, nullptr);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, internalFormat, type, nullptr);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, internalFormat, type, nullptr);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    TextureCube::TextureCube(const std::string &path) {

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

    bool TextureCube::isLoaded() const {
        return loaded;
    }
}
