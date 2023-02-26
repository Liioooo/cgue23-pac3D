#include "RenderPass.h"

namespace CgEngine {
    RenderPass::RenderPass(RenderPassSpecification spec) : specification(spec) {}

    RenderPass::~RenderPass() {
        delete specification.framebuffer;
    }

    RenderPassSpecification& RenderPass::getSpecification() {
        return specification;
    }
}
