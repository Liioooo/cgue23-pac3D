#pragma once

#include "Shader.h"
#include "Framebuffer.h"

namespace CgEngine {

    enum class DepthCompareOperator {
        Never =             0x0200, // GL_NEVER
        Less =              0x0201, // GL_LESS
        Equal =             0x0202, // GL_EQUAL
        LessOrEqual =       0x0203, // GL_LEQUAL
        Greater =           0x0204, // GL_GREATER
        NotEqual =          0x0205, // GL_NOTEQUAL
        GreaterOrEqual =    0x0206, // GL_GEQUAL
        Always =            0x0207, // GL_ALWAYS
    };

    struct RenderPassSpecification {
        Shader* shader = nullptr;
        DepthCompareOperator depthCompareOperator = DepthCompareOperator::Less;
        Framebuffer* framebuffer = nullptr;
        bool clearDepthBuffer = true;
        bool clearColorBuffer = true;
        bool clearStencilBuffer = false;
        bool backfaceCulling = true;
        bool depthTest = true;
        bool depthWrite = true;
        bool wireframe = false;
    };

    class RenderPass {
    public:
        explicit RenderPass(RenderPassSpecification spec);
        ~RenderPass();

        RenderPassSpecification& getSpecification();

    private:
        RenderPassSpecification specification;
    };

}
