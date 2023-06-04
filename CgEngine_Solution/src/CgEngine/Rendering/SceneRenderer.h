#pragma once

#include "RenderPass.h"
#include "Material.h"
#include "Camera.h"
#include "UniformBuffer.h"
#include "VertexArrayObject.h"
#include "Scene/Scene.h"
#include "Renderer.h"

namespace CgEngine {

    class Scene;

    class SceneRenderer {
    public:
        explicit SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight);
        ~SceneRenderer();

        void setActiveScene(Scene* scene);
        void setViewportSize(uint32_t width, uint32_t height);
        void beginScene(const Camera& camera, glm::mat4 cameraTransform, const SceneLightEnvironment& lightEnvironment, const SceneEnvironment& sceneEnvironment);
        void endScene();
        void submitMesh(MeshVertices& mesh, const std::vector<uint32_t>& meshNodes, Material* overrideMaterial, bool castShadows, const glm::mat4& transform);
        void submitAnimatedMesh(MeshVertices& mesh, const std::vector<uint32_t>& meshNodes, Material* overrideMaterial, bool castShadows, const glm::mat4& transform, const std::vector<glm::mat4>& boneTransforms, VertexArrayObject* skinnedVAO);
        void submitUiElements(const std::unordered_map<std::string, UiElement*>& uiElements);
        void submitPhysicsColliderMesh(MeshVertices& mesh, const glm::mat4& transform);
        void submitDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);

    private:
        static const uint32_t maxBones = 100;
        static const uint32_t maxAnimatedComponents = 512;

        Scene* activeScene;
        uint32_t viewportWidth;
        uint32_t viewportHeight;
        bool needsResize = true;
        bool activeRendering = false;

        RenderPass* shadowMapRenderPass;
        RenderPass* preDepthRenderPass;
        RenderPass* geometryRenderPass;
        RenderPass* screenRenderPass;
        RenderPass* skyboxRenderPass;
        RenderPass* bloomDownSamplePass;
        RenderPass* bloomUpSamplePass;
        RenderPass* uiCirclePass;
        RenderPass* uiRectPass;
        RenderPass* uiTextPass;
        RenderPass* physicsCollidersRenderPass;
        RenderPass* normalsDebugRenderPass;
        RenderPass* debugLinesRenderPass;
        Material* screenMaterial;
        Material* skyboxMaterial;
        Material* physicsCollidersMaterial;
        Material* normalsDebugMaterial;
        Material* emptyMaterial;

        Texture2DArray* dirShadowMaps;
        std::array<Texture2D*, 7> bloomTextures;

        void skinMeshes();
        void shadowMapPass();
        void preDepthPass();
        void geometryPass();
        void skyboxPass();
        void physicsCollidersPass();
        void normalsDebugPass();
        void debugLinesPass();
        void bloomPass();
        void screenPass();
        void uiPass();
        void clearPass(RenderPass& renderPass);

        void setupShadowMapData(glm::vec3 dirLightDirection, const glm::mat4& cameraViewProjection, const Camera& camera);

        struct UBCameraData {
            glm::mat4 viewProjection;
            glm::mat4 projection;
            glm::mat4 view;
            glm::mat4 uiProjectionMatrix;
            glm::vec3 position;
            float exposure;
            float bloomIntensity;
            float bloomThreshold;
            glm::vec2 _padding_;
        };
        UniformBuffer<UBCameraData>* ubCameraData;

        struct UBPointLight {
            glm::vec3 position;
            float intensity;
            glm::vec3 color;
            float radius;
            float falloff;
            glm::vec3 _padding0_;
        };

        struct UBSpotLight {
            glm::vec3 position;
            float intensity;
            glm::vec3 color;
            float radius;
            glm::vec3 direction;
            float falloff;
            float innerAngle;
            float outerAngle;
            glm::vec2 _padding0_;
        };

        struct UBLightData {
            glm::vec3 dirLightDirection;
            float dirLightIntensity;
            glm::vec3 dirLightColor;
            int pointLightCount;
            int spotLightCount;
            glm::vec3 _padding0_;
            UBPointLight pointLights[100];
            UBSpotLight spotLights[100];
        };
        UniformBuffer<UBLightData>* ubLightData;

        struct UBDirShadowData {
            glm::mat4 lightSpaceMat[4];
            glm::vec4 cascadeSplits;
        };
        UniformBuffer<UBDirShadowData>* ubDirShadowData;

        struct MeshKey {
            const uint32_t voaId;
            const uint32_t submeshIndex;
            const uint32_t materialUuid;

            bool operator<(const MeshKey& other) const {
                if (voaId < other.voaId) return true;
                if (voaId > other.voaId) return false;
                if (submeshIndex < other.submeshIndex) return true;
                if (submeshIndex > other.submeshIndex) return false;
                return materialUuid < other.materialUuid;
            }
        };

        struct DrawCommand {
            VertexArrayObject* vao;
            const Material* material;
            uint32_t baseIndex;
            uint32_t baseVertex;
            uint32_t indexCount;
            uint32_t instanceCount;
        };

        std::map<MeshKey, DrawCommand> drawCommandQueue;
        std::map<MeshKey, DrawCommand> shadowMapDrawCommandQueue;
        std::map<MeshKey, std::vector<glm::mat4>> meshTransforms;

        std::map<MeshKey, DrawCommand> physicsCollidersDrawCommandQueue;
        std::map<MeshKey, std::vector<glm::mat4>> physicsCollidersMeshTransforms;

        std::vector<LineDrawInfo> debugLinesDrawInfoQueue;

        struct SkinningInfo {
            const VertexBuffer* originalVertexBuffer;
            const VertexBuffer* skinnedVertexBuffer;
            const ShaderStorageBuffer* boneInfluencesBuffer;
            uint32_t numVertices;
        };

        std::vector<SkinningInfo> skinningQueue;

        struct CurrentSceneEnvironment {
            uint32_t prefilterMapId;
            uint32_t irradianceMapId;
            float environmentIntensity;
            bool dirLightCastShadows;
        } currentSceneEnvironment;

        struct UiDrawInfo {
            uint32_t circleIndexCount = 0;
            std::vector<UiCircleVertex> circleVertices;
            uint32_t rectIndexCount = 0;
            std::vector<UiRectVertex> rectVertices;
            std::array<const Texture2D*, Renderer::maxTextureSlots> textureSlots{};
            uint32_t filledTextureSlots = 0;

            std::vector<UiTextVertex> textVertices;
            uint32_t textIndexCount = 0;
            std::array<const Texture2D*, 4> fontAtlases;
            uint32_t filledFontAtlases = 0;
        };

        std::map<uint32_t, UiDrawInfo> uiDrawInfoQueue;

        glm::mat4 uiProjectionMatrix;

        ShaderStorageBuffer* boneTransformsBuffer;

        float findDrawInfoTextureIndex(UiDrawInfo& drawInfo, const Texture2D* texture) const;
    };

}
