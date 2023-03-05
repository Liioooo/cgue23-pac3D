#pragma once

#include "RenderPass.h"
#include "Material.h"
#include "VertexArrayObject.h"
#include "ShaderStorageBuffer.h"

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
        static void renderFullScreenQuad(const Material& material);
        static void renderUnitCube(const Material& material);
        static void executeDrawCommand(const VertexArrayObject& vao, const Material& material, uint32_t indexCount, uint32_t baseIndex, uint32_t baseVertex, const std::vector<glm::mat4>& transforms, uint32_t instanceCount);

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

        static ShaderStorageBuffer* transformsBuffer;
    };

}
