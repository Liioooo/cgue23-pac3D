#pragma once
#include "glad/glad.h"

namespace CgEngine {

    enum class TextureFormat {
        RGB, RGBA, Float16, Float32
    };

    enum class TextureWrap {
        Repeat, Clamp
    };

    enum class MipMapFiltering {
        Nearest, Bilinear, Trilinear
    };

    namespace TextureUtils {
        static GLint getOpenGLTextureFormat(TextureFormat format) {
            switch (format) {
                case TextureFormat::RGB:     return GL_RGB;
                case TextureFormat::RGBA:    return GL_RGBA;
                case TextureFormat::Float16: return GL_RGBA16F;
                case TextureFormat::Float32: return GL_RGBA32F;
            }
            return 0;
        }

        static GLint getTextureFormatType(TextureFormat format) {
            if (format == TextureFormat::Float16 || format == TextureFormat::Float32) {
                return GL_FLOAT;
            }
            return GL_UNSIGNED_BYTE;
        }

        static void applyMipMapFiltering(MipMapFiltering mipMapFiltering, GLenum textureType) {
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

        static uint32_t calculateMipCount(uint32_t width, uint32_t height) {
            return static_cast<uint32_t>(std::floor(std::log2(glm::min(width, height))) + 1);
        }
    }

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual TextureFormat getFormat() const = 0;
        virtual uint32_t getRendererId() const = 0;
        virtual void bind(uint32_t slot) = 0;
        virtual void generateMipMaps() = 0;
    };

    class Texture2D : public Texture {
    public:
        static Texture2D* createResource(const std::string& name);

        Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, MipMapFiltering mipMapFiltering = MipMapFiltering::Trilinear);
        Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, const void* data, MipMapFiltering mipMapFiltering = MipMapFiltering::Trilinear);
        Texture2D(const std::string& path, bool srgb, MipMapFiltering mipMapFiltering = MipMapFiltering::Trilinear);
        ~Texture2D() override;

        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        TextureFormat getFormat() const override;
        uint32_t getRendererId() const override;
        void bind(uint32_t slot) override;
        void generateMipMaps() override;
        bool isLoaded() const;

    private:
        uint32_t id;
        uint32_t width;
        uint32_t height;
        TextureFormat format;
        bool loaded = false;
    };

    class TextureCube : public Texture {
    public:
        static TextureCube* createResource(const std::string& name);

        TextureCube(TextureFormat format, uint32_t width, uint32_t height, MipMapFiltering mipMapFiltering = MipMapFiltering::Bilinear);
        TextureCube(TextureFormat format, uint32_t width, uint32_t height, const void* data, MipMapFiltering mipMapFiltering = MipMapFiltering::Bilinear);
        ~TextureCube() override;

        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        TextureFormat getFormat() const override;
        uint32_t getRendererId() const override;
        void bind(uint32_t slot) override;
        void generateMipMaps() override;

    private:
        uint32_t id;
        uint32_t width;
        uint32_t height;
        TextureFormat format;
    };

}
