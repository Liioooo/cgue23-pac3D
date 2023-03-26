#pragma once

namespace CgEngine {

    enum class FramebufferFormat {
        RGBA8, RGBA16F
    };

    struct FramebufferSpecification {
        uint32_t width;
        uint32_t height;
        glm::vec4 clearColor;
        bool hasDepthStencilAttachment = true;
        bool hasDepthAttachment = false;
        bool useExistingDepthStencilAttachment = false;
        bool useExistingDepthAttachment = false;
        uint32_t existingDepthAttachment;
        std::vector<FramebufferFormat> colorAttachments;
        uint32_t samples = 1;
        bool screenTarget = false;
    };

    class Framebuffer {
    public:
        explicit Framebuffer(FramebufferSpecification spec);
        ~Framebuffer();

        void bind();
        void unbind();
        void resize(uint32_t width, uint32_t height, bool forceRecreate);
        uint32_t getRendererId() const;
        uint32_t getColorAttachmentRendererId(size_t index) const;
        uint32_t getDepthAttachmentRendererId() const;
        FramebufferSpecification& getSpecification();

    private:
        FramebufferSpecification specification;
        uint32_t id = 0;
        std::vector<uint32_t> colorAttachments;
        uint32_t depthAttachment = 0;
    };

}
