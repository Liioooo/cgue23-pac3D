#pragma once

#include "RenderPass.h"
#include "Material.h"
#include "Camera.h"
#include "UniformBuffer.h"
#include "VertexArrayObject.h"
#include "Scene/Scene.h"

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
        void submitMesh(MeshVertices& mesh, const std::vector<uint32_t>& submeshIndices, Material* overrideMaterial, const glm::mat4& transform);
        void submitPhysicsColliderMesh(MeshVertices& mesh, const glm::mat4& transform);

    private:
        Scene* activeScene;
        uint32_t viewportWidth;
        uint32_t viewportHeight;
        bool needsResize = true;
        bool activeRendering = false;

        RenderPass* geometryRenderPass;
        RenderPass* screenRenderPass;
        RenderPass* skyboxRenderPass;
        RenderPass* physicsCollidersRenderPass;
        RenderPass* normalsDebugRenderPass;
        Material* screenMaterial;
        Material* skyboxMaterial;
        Material* physicsCollidersMaterial;
        Material* normalsDebugMaterial;

        void geometryPass();
        void skyboxPass();
        void physicsCollidersPass();
        void normalsDebugPass();
        void screenPass();
        void clearPass(RenderPass& renderPass);

        struct UBCameraData {
            glm::mat4 viewProjection;
            glm::mat4 projection;
            glm::mat4 view;
            glm::vec3 position;
            float _padding0_;
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
        std::map<MeshKey, std::vector<glm::mat4>> meshTransforms;

        std::map<MeshKey, DrawCommand> physicsCollidersDrawCommandQueue;
        std::map<MeshKey, std::vector<glm::mat4>> physicsCollidersMeshTransforms;

        struct CurrentSceneEnvironment {
            uint32_t prefilterMapId;
            uint32_t irradianceMapId;
            float environmentIntensity;
        } currentSceneEnvironment;

    };

}
