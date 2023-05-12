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

    struct UiCircleVertex {
        glm::vec4 posUV;
        glm::vec4 lineColor;
        glm::vec4 fillColor;
        float lineWidth;
    };

    struct UiDrawInfo {
        uint32_t circleIndexCount = 0;
        std::vector<UiCircleVertex> circleVertices;
    };

    class Renderer {
    public:
        static void init();
        static void shutdown();
        static void beginRenderPass(RenderPass& renderPass);
        static void endRenderPass();
        static void renderUnitQuad(const Material& material);
        static void renderUnitCube(const Material& material);
        static void renderLines(const std::vector<LineDrawInfo>& lines);
        static void executeDrawCommand(const VertexArrayObject& vao, const Material& material, uint32_t indexCount, uint32_t baseIndex, uint32_t baseVertex, const std::vector<glm::mat4>& transforms, uint32_t instanceCount);
        static void renderUiCircles(const std::vector<UiCircleVertex>& vertices, uint32_t indexCount);

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
        static bool useBlending;
        static BlendingEquation blendingEquation;
        static BlendingFunction srcBlendingFunction;
        static BlendingFunction destBlendingFunction;

        static Texture2D* whiteTexture;
        static Texture2D* brdfLUT;
        static TextureCube* blackCubeTexture;

        static VertexArrayObject* quadVAO;
        static VertexArrayObject* unitCubeVAO;
        static VertexArrayObject* linesVAO;

        static VertexArrayObject* uiCircleVAO;

        static ShaderStorageBuffer* transformsBuffer;
    };

}
