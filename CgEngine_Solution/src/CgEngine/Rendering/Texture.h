#pragma once
#include "glad/glad.h"

namespace CgEngine {

    enum class TextureFormat {
        RGB, RGBA, Float16
    };

    enum class TextureWrap {
        Repeat, Clamp
    };

    static GLint getOpenGLTextureFormat(TextureFormat format) {
        switch (format) {
            case TextureFormat::RGB:     return GL_RGB;
            case TextureFormat::RGBA:    return GL_RGBA;
            case TextureFormat::Float16: return GL_RGBA16F;
        }
        return 0;
    }

    class Texture {
    public:
        virtual ~Texture() = default;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual TextureFormat getFormat() const = 0;
        virtual uint32_t getRendererId() const = 0;
        virtual void bind(uint32_t slot) = 0;
        virtual bool isLoaded() const = 0;
    };

    class Texture2D : public Texture {
    public:
        static Texture2D* createResource(const std::string& name);

        Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap);
        Texture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, const void* data);
        Texture2D(const std::string& path, bool srgb);
        ~Texture2D() override;

        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        TextureFormat getFormat() const override;
        uint32_t getRendererId() const override;
        void bind(uint32_t slot) override;
        bool isLoaded() const override;

    private:
        uint32_t id;
        uint32_t width;
        uint32_t height;
        TextureFormat format;
        bool loaded = false;
    };

    class TextureCube : public Texture {
    public:
        TextureCube(TextureFormat format, uint32_t width, uint32_t height);
        explicit TextureCube(const std::string& path);
        ~TextureCube() override;

        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        TextureFormat getFormat() const override;
        uint32_t getRendererId() const override;
        void bind(uint32_t slot) override;
        bool isLoaded() const override;

    private:
        uint32_t id;
        uint32_t width;
        uint32_t height;
        TextureFormat format;
        bool loaded = false;
    };

}
