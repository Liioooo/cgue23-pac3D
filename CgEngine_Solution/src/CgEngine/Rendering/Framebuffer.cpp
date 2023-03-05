#include <Asserts.h>

#include <utility>
#include "Framebuffer.h"
#include "glad/glad.h"

namespace CgEngine {
    Framebuffer::Framebuffer(FramebufferSpecification spec) : specification(std::move(spec)) {
        resize(specification.width, specification.height, true);
    }

    Framebuffer::~Framebuffer() {
        if (!specification.screenTarget) {
            glDeleteFramebuffers(1, &id);
            for (const auto &item: colorAttachments) {
                glDeleteTextures(1, &item);
            }
            if (specification.hasDepthStencilAttachment) {
                glDeleteTextures(1, &depthAttachment);
            }
        }
    }

    void Framebuffer::bind() {
        if (specification.screenTarget) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
        }
    }

    void Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::resize(uint32_t width, uint32_t height, bool forceRecreate) {
        if (!forceRecreate && (specification.width == width && specification.height == height)) {
            return;
        }

        specification.width = width;
        specification.height = height;

        if (specification.screenTarget) {
            glViewport(0, 0, width, height);
            return;
        }

        if (id) {
            glDeleteFramebuffers(1, &id);
            for (const auto &item: colorAttachments) {
                glDeleteTextures(1, &item);
            }
            if (specification.hasDepthStencilAttachment) {
                glDeleteTextures(1, &depthAttachment);
            }
        }

        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        bool multisample = specification.samples > 1;

        uint32_t index = 0;
        for (const auto &format: specification.colorAttachments) {

            colorAttachments.push_back(0);

            if (multisample) {
                glGenTextures(1, &colorAttachments.back());
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorAttachments.back());
                if (format == FramebufferFormat::RGBA16F) {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification.samples, GL_RGBA16F, specification.width, specification.height, GL_FALSE);
                } else {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification.samples, GL_RGBA8, specification.width, specification.height, GL_FALSE);
                }
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D_MULTISAMPLE, colorAttachments.back(), 0);
            } else {
                glGenTextures(1, &colorAttachments.back());
                glBindTexture(GL_TEXTURE_2D, colorAttachments.back());
                if (format == FramebufferFormat::RGBA16F) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, specification.width, specification.height, 0, GL_RGBA, GL_FLOAT, nullptr);
                } else {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification.width, specification.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                }
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);

                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, colorAttachments.back(), 0);
            }

            index++;
        }


        if (specification.hasDepthStencilAttachment) {
            if (multisample) {
                glGenTextures(1, &depthAttachment);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthAttachment);
                glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, specification.samples, GL_DEPTH24_STENCIL8, specification.width, specification.height, GL_FALSE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            } else {
                glGenTextures(1, &depthAttachment);
                glBindTexture(GL_TEXTURE_2D, depthAttachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specification.width, specification.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthAttachment, 0);
        }

        CG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    uint32_t Framebuffer::getRendererId() const {
        return id;
    }

    uint32_t Framebuffer::getColorAttachmentRendererId(size_t index) const {
        return colorAttachments[index];
    }

    uint32_t Framebuffer::getDepthAttachmentRendererId() const {
        return depthAttachment;
    }

    FramebufferSpecification &Framebuffer::getSpecification() {
        return specification;
    }
}
