#pragma once

#include "RenderPass.h"
#include "Material.h"
#include "VertexArrayObject.h"

namespace CgEngine {

    struct RendererData {
        VertexArrayObject* quadVAO;
    };

    class Renderer {
    public:
        static void init();
        static void shutdown();
        static void beginRenderPass(RenderPass& renderPass);
        static void endRenderPass(RenderPass& renderPass);
        static void renderFullScreenQuad(RenderPass& renderPass, Material& material);

        static Texture2D& getWhiteTexture();

    private:
        static RendererData* rendererData;
        static bool isWireframe;
        static bool isBackfaceCulling;
        static DepthCompareOperator depthCompareOperator;
        static bool depthTest;
        static bool depthWrite;

        static Texture2D* whiteTexture;
    };

}
