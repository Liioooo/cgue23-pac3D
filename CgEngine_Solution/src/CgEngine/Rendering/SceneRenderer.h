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
        void beginScene(const Camera& camera, glm::mat4 cameraTransform, const SceneLightEnvironment& lightEnvironment);
        void endScene();
        void submitMesh(VertexArrayObject& vao, const Material& material, const glm::mat4& transform);

    private:
        Scene* activeScene;
        uint32_t viewportWidth;
        uint32_t viewportHeight;
        bool needsResize = true;
        bool activeRendering = false;

        RenderPass* geometryRenderPass;
        RenderPass* screenRenderPass;
        Material* screenMaterial;

        void geometryPass();
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

        struct RenderData {
            VertexArrayObject& vao;
            const Material& material;
            const glm::mat4& transform;
        };
        std::vector<RenderData> geoQueue{};
    };

}
