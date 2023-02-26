#pragma once

#include "glad/glad.h"
#include "Shader.h"
#include "Framebuffer.h"

namespace CgEngine {

    enum class DepthCompareOperator {
        Never = GL_NEVER,
        NotEqual = GL_NOTEQUAL,
        Less = GL_LESS,
        LessOrEqual = GL_LEQUAL,
        Greater = GL_GREATER,
        GreaterOrEqual = GL_GEQUAL,
        Equal = GL_EQUAL,
        Always = GL_ALWAYS,
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
