#pragma once

#include "RenderPass.h"
#include "Material.h"
#include "VertexArrayObject.h"
#include "ShaderStorageBuffer.h"

namespace CgEngine {

    struct LineDrawInfo {
        glm::vec3 from;
        glm::vec3 to;
        glm::vec3 color;
    };

    class Renderer {
    public:
        static void init();
        static void shutdown();
        static void beginRenderPass(RenderPass& renderPass);
        static void endRenderPass();
        static void renderFullScreenQuad(const Material& material);
        static void renderUnitCube(const Material& material);
        static void renderLines(const std::vector<LineDrawInfo>& lines);
        static void executeDrawCommand(const VertexArrayObject& vao, const Material& material, uint32_t indexCount, uint32_t baseIndex, uint32_t baseVertex, const std::vector<glm::mat4>& transforms, uint32_t instanceCount);

        static Texture2D& getWhiteTexture();
        static Texture2D& getBrdfLUTTexture();
        static TextureCube& getBlackCubeTexture();
        static Material* getDefaultPBRMaterial();
        static std::pair<TextureCube*, TextureCube*> createEnvironmentMap(const std::string& hdriPath);

    private:
        static RenderPass* currentRenderPass;
        static bool isWireframe;
        static bool isBackFaceCulling;
        static bool isFrontFaceCulling;
        static DepthCompareOperator depthCompareOperator;
        static bool depthTest;
        static bool depthWrite;

        static Texture2D* whiteTexture;
        static Texture2D* brdfLUT;
        static TextureCube* blackCubeTexture;

        static VertexArrayObject* quadVAO;
        static VertexArrayObject* unitCubeVAO;
        static VertexArrayObject* linesVAO;

        static ShaderStorageBuffer* transformsBuffer;
    };

}
