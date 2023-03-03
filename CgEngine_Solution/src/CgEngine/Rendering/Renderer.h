#pragma once

#include "RenderPass.h"
#include "Material.h"
#include "VertexArrayObject.h"

namespace CgEngine {

    struct RendererData {
        std::unique_ptr<VertexArrayObject> quadVAO;
        std::unique_ptr<VertexArrayObject> unitCubeVAO;
    };

    class Renderer {
    public:
        static void init();
        static void shutdown();
        static void beginRenderPass(RenderPass& renderPass);
        static void endRenderPass();
        static void renderFullScreenQuad(Material& material);
        static void renderUnitCube(Material& material);

        static Texture2D& getWhiteTexture();
        static Texture2D& getBrdfLUTTexture();
        static TextureCube& getBlackCubeTexture();
        static Material* getDefaultPBRMaterial();
        static std::pair<TextureCube*, TextureCube*> createEnvironmentMap(const std::string& hdriPath);

    private:
        static RenderPass* currentRenderPass;
        static RendererData rendererData;
        static bool isWireframe;
        static bool isBackfaceCulling;
        static DepthCompareOperator depthCompareOperator;
        static bool depthTest;
        static bool depthWrite;

        static Texture2D* whiteTexture;
        static Texture2D* brdfLUT;
        static TextureCube* blackCubeTexture;
    };

}
