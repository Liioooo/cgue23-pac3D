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
        float width;
        float lineWidth;
        float textureIndex;
    };

    struct UiRectVertex {
        glm::vec4 posUV;
        glm::vec4 lineColor;
        glm::vec4 fillColor;
        glm::vec2 size;
        float lineWidth;
        float textureIndex;
    };

    struct UiTextVertex {
        glm::vec4 posUV;
        glm::vec4 color;
        float fontAtlasIndex;
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
        static void renderUiRects(const std::vector<UiRectVertex>& vertices, uint32_t indexCount);
        static void renderUiText(const std::vector<UiTextVertex>& vertices, uint32_t indexCount);

        static Texture2D& getWhiteTexture();
        static Texture2D& getBrdfLUTTexture();
        static TextureCube& getBlackCubeTexture();
        static Material* getDefaultPBRMaterial();
        static std::pair<TextureCube*, TextureCube*> createEnvironmentMap(const std::string& hdriPath);

        static const uint32_t maxUiQuads = 5000;
        static const uint32_t maxUiIndices = maxUiQuads * 6;

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
        static VertexArrayObject* uiRectVAO;
        static VertexArrayObject* uiTextVAO;

        static ShaderStorageBuffer* transformsBuffer;
    };

}
