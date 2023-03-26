#include "SceneRenderer.h"
#include "Asserts.h"
#include "GlobalObjectManager.h"
#include "Renderer.h"
#include "Application.h"

namespace CgEngine {
    SceneRenderer::SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight) : viewportWidth(viewportWidth), viewportHeight(viewportHeight) {
        {
            FramebufferSpecification shadowMapFramebufferSpec;
            shadowMapFramebufferSpec.height = 2048;
            shadowMapFramebufferSpec.width = 2048;
            shadowMapFramebufferSpec.clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
            shadowMapFramebufferSpec.hasDepthStencilAttachment = false;
            shadowMapFramebufferSpec.hasDepthAttachment = true;

            auto* framebuffer = new Framebuffer(shadowMapFramebufferSpec);

            RenderPassSpecification shadowMapRenderPassSpec;
            shadowMapRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("dirShadowMap");
            shadowMapRenderPassSpec.framebuffer = framebuffer;
            shadowMapRenderPassSpec.clearColorBuffer = false;
            shadowMapRenderPassSpec.clearDepthBuffer = true;
            shadowMapRenderPassSpec.clearStencilBuffer = false;
            shadowMapRenderPassSpec.frontfaceCulling = false;
            shadowMapRenderPassSpec.backfaceCulling = true;

            shadowMapRenderPass = new RenderPass(shadowMapRenderPassSpec);

            shadowMapMaterial = new Material("ShadowMapMaterial");
        }
        {
            FramebufferSpecification geoFramebufferSpec;
            geoFramebufferSpec.height = viewportHeight;
            geoFramebufferSpec.width = viewportWidth;
            geoFramebufferSpec.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            geoFramebufferSpec.colorAttachments = {FramebufferFormat::RGBA16F};
            geoFramebufferSpec.samples = 1;

            auto* framebuffer = new Framebuffer(geoFramebufferSpec);

            RenderPassSpecification geoRenderPassSpec;
            geoRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("pbr");
            geoRenderPassSpec.framebuffer = framebuffer;

            geometryRenderPass = new RenderPass(geoRenderPassSpec);
        }
        {
            RenderPassSpecification skyboxRenderPassSpec;
            skyboxRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("skybox");
            skyboxRenderPassSpec.framebuffer = geometryRenderPass->getSpecification().framebuffer;
            skyboxRenderPassSpec.depthCompareOperator = DepthCompareOperator::LessOrEqual;
            skyboxRenderPassSpec.clearColorBuffer = false;
            skyboxRenderPassSpec.clearDepthBuffer = false;
            skyboxRenderPassSpec.clearStencilBuffer = false;

            skyboxRenderPass = new RenderPass(skyboxRenderPassSpec);

            skyboxMaterial = new Material("skyboxMaterial");
        }
        {
            RenderPassSpecification physicsCollidersRenderPassSpec;
            physicsCollidersRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("physicsColliders");
            physicsCollidersRenderPassSpec.framebuffer = geometryRenderPass->getSpecification().framebuffer;
            physicsCollidersRenderPassSpec.depthTest = false;
            physicsCollidersRenderPassSpec.depthWrite = false;
            physicsCollidersRenderPassSpec.clearColorBuffer = false;
            physicsCollidersRenderPassSpec.clearDepthBuffer = false;
            physicsCollidersRenderPassSpec.clearStencilBuffer = false;
            physicsCollidersRenderPassSpec.wireframe = true;

            physicsCollidersRenderPass = new RenderPass(physicsCollidersRenderPassSpec);

            physicsCollidersMaterial = new Material("PhysicsColliderDebugMaterial");
            physicsCollidersMaterial->set("u_Color", {0.0f, 1.0f, 0.0f});
        }
        {
            RenderPassSpecification mormalsDebugRenderPassSpec;
            mormalsDebugRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("normalsVisualize");
            mormalsDebugRenderPassSpec.framebuffer = geometryRenderPass->getSpecification().framebuffer;
            mormalsDebugRenderPassSpec.depthTest = true;
            mormalsDebugRenderPassSpec.depthWrite = false;
            mormalsDebugRenderPassSpec.clearColorBuffer = false;
            mormalsDebugRenderPassSpec.clearDepthBuffer = false;
            mormalsDebugRenderPassSpec.clearStencilBuffer = false;

            normalsDebugRenderPass = new RenderPass(mormalsDebugRenderPassSpec);

            normalsDebugMaterial = new Material("NormalsDebugMaterial");
            normalsDebugMaterial->set("u_Color", {1.0f, 0.0f, 0.0f});
        }
        {
            FramebufferSpecification screenFramebufferSpec;
            screenFramebufferSpec.height = viewportHeight;
            screenFramebufferSpec.width = viewportWidth;
            screenFramebufferSpec.clearColor = {0.0f, 1.0f, 1.0f, 1.0f};
            screenFramebufferSpec.hasDepthStencilAttachment = false;
            screenFramebufferSpec.samples = 1;
            screenFramebufferSpec.screenTarget = true;

            auto* framebuffer = new Framebuffer(screenFramebufferSpec);
            RenderPassSpecification screenRenderPassSpec;
            screenRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("screen");
            screenRenderPassSpec.framebuffer = framebuffer;
            screenRenderPassSpec.depthTest = false;

            screenRenderPass = new RenderPass(screenRenderPassSpec);

            screenMaterial = new Material("screenMaterial");
            screenMaterial->setTexture2D("u_FinalImage", geometryRenderPass->getSpecification().framebuffer->getColorAttachmentRendererId(0), 0);
        }

        ubCameraData = new UniformBuffer<UBCameraData>("CameraData", 0, *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("pbr"));
        ubLightData = new UniformBuffer<UBLightData>("LightData", 1, *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("pbr"));
        ubDirShadowData = new UniformBuffer<UBDirShadowData>("DirShadowData", 2, *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("dirShadowMap"));
    }

    SceneRenderer::~SceneRenderer() {
        delete shadowMapRenderPass;
        delete geometryRenderPass;
        delete skyboxRenderPass;
        delete physicsCollidersRenderPass;
        delete normalsDebugRenderPass;
        delete screenRenderPass;

        delete skyboxMaterial;
        delete physicsCollidersMaterial;
        delete normalsDebugMaterial;
        delete screenMaterial;
        delete shadowMapMaterial;

        delete ubCameraData;
        delete ubLightData;
        delete ubDirShadowData;
    }

    void SceneRenderer::setActiveScene(Scene* scene) {
        activeScene = scene;
    }

    void SceneRenderer::setViewportSize(uint32_t width, uint32_t height) {
        if (width != viewportWidth || height != viewportHeight) {
            needsResize = true;

            viewportWidth = width;
            viewportHeight = height;
        }
    }

    void SceneRenderer::beginScene(const Camera& camera, glm::mat4 cameraTransform, const SceneLightEnvironment& lightEnvironment, const SceneEnvironment& sceneEnvironment) {
        CG_ASSERT(!activeRendering, "Already Rendering Scene!")
        CG_ASSERT(activeScene, "No active Scene!")

        activeRendering = true;

        if (needsResize) {
            needsResize = false;

            geometryRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);
            screenRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);
        }

        UBCameraData cameraData{};
        cameraData.projection = camera.getProjectionMatrix();
        cameraData.view = glm::inverse(cameraTransform);
        cameraData.viewProjection = cameraData.projection * cameraData.view;
        cameraData.position = cameraTransform[3];
        ubCameraData->setData(cameraData);

        UBLightData lightData{};
        lightData.dirLightDirection = lightEnvironment.dirLightDirection;
        lightData.dirLightIntensity = lightEnvironment.dirLightIntensity;
        lightData.dirLightColor = lightEnvironment.dirLightColor;
        lightData.pointLightCount = lightEnvironment.pointLights.size();
        lightData.spotLightCount = lightEnvironment.spotLights.size();

        size_t indexPL = 0;
        for (const auto &pointLight: lightEnvironment.pointLights) {
            lightData.pointLights[indexPL].color = pointLight.color;
            lightData.pointLights[indexPL].falloff = pointLight.falloff;
            lightData.pointLights[indexPL].intensity = pointLight.intensity;
            lightData.pointLights[indexPL].radius = pointLight.radius;
            lightData.pointLights[indexPL].position = pointLight.position;
            indexPL++;
        }

        size_t indexSL = 0;
        for (const auto &spotLight: lightEnvironment.spotLights) {
            lightData.spotLights[indexSL].color = spotLight.color;
            lightData.spotLights[indexSL].falloff = spotLight.falloff;
            lightData.spotLights[indexSL].intensity = spotLight.intensity;
            lightData.spotLights[indexSL].radius = spotLight.radius;
            lightData.spotLights[indexSL].position = spotLight.position;
            lightData.spotLights[indexSL].position = spotLight.position;
            lightData.spotLights[indexSL].direction = spotLight.direction;
            lightData.spotLights[indexSL].innerAngle = spotLight.innerAngle;
            lightData.spotLights[indexSL].outerAngle = spotLight.outerAngle;
            indexSL++;
        }

        ubLightData->setData(lightData);

        skyboxMaterial->setTextureCube("u_Texture", sceneEnvironment.prefilterMap->getRendererId(), 0);
        skyboxMaterial->set("u_Intensity", sceneEnvironment.environmentIntensity);
        skyboxMaterial->set("u_Lod", sceneEnvironment.environmentLod);

        currentSceneEnvironment.environmentIntensity = sceneEnvironment.environmentIntensity;
        currentSceneEnvironment.irradianceMapId = sceneEnvironment.irradianceMap->getRendererId();
        currentSceneEnvironment.prefilterMapId = sceneEnvironment.prefilterMap->getRendererId();
        currentSceneEnvironment.dirLightCastShadows = lightEnvironment.dirLightCastShadows && lightEnvironment.dirLightIntensity != 0.0f;

        setupShadowMapData(lightEnvironment.dirLightDirection);
    }

    void SceneRenderer::endScene() {
        CG_ASSERT(activeRendering, "Not actively rendering!")

        ApplicationOptions& applicationOptions = Application::get().getApplicationOptions();

        shadowMapPass();
        geometryPass();
        skyboxPass();
        if (applicationOptions.debugShowPhysicsColliders) {
            physicsCollidersPass();
        }
        if (applicationOptions.debugShowNormals) {
            normalsDebugPass();
        }
        screenPass();

        drawCommandQueue.clear();
        shadowMapDrawCommandQueue.clear();
        meshTransforms.clear();

        physicsCollidersDrawCommandQueue.clear();
        physicsCollidersMeshTransforms.clear();

        activeRendering = false;
    }

    void SceneRenderer::submitMesh(MeshVertices& mesh, const std::vector<uint32_t>& submeshIndices, Material* overrideMaterial, bool castShadows, const glm::mat4& transform) {
        auto& submeshes = mesh.getSubmeshes();

        for (const auto &index: submeshIndices) {
            const Submesh& submesh = submeshes.at(index);

            const Material* material = overrideMaterial != nullptr ? overrideMaterial : mesh.getMaterial(submesh.materialIndex);

            MeshKey mk = {mesh.getVAO()->getRendererId(), index, material->getUuid().getUuid()};

            meshTransforms[mk].emplace_back(transform * submesh.transform);

            DrawCommand& drawCommand = drawCommandQueue[mk];
            drawCommand.vao = mesh.getVAO();
            drawCommand.material = material;
            drawCommand.baseIndex = submesh.baseIndex;
            drawCommand.baseVertex = submesh.baseVertex;
            drawCommand.indexCount = submesh.indexCount;
            drawCommand.instanceCount++;

            if (castShadows) {
                DrawCommand& shadowMapDrawCommand = shadowMapDrawCommandQueue[mk];
                shadowMapDrawCommand.vao = mesh.getVAO();
                shadowMapDrawCommand.material = material;
                shadowMapDrawCommand.baseIndex = submesh.baseIndex;
                shadowMapDrawCommand.baseVertex = submesh.baseVertex;
                shadowMapDrawCommand.indexCount = submesh.indexCount;
                shadowMapDrawCommand.instanceCount++;
            }
        }
    }

    void SceneRenderer::submitPhysicsColliderMesh(MeshVertices& mesh, const glm::mat4& transform) {
        const auto& submeshes = mesh.getSubmeshes();
        for (uint32_t i = 0; i < submeshes.size(); i++) {
            const auto& submesh = submeshes.at(i);

            MeshKey mk = {mesh.getVAO()->getRendererId(), i, physicsCollidersMaterial->getUuid().getUuid()};

            physicsCollidersMeshTransforms[mk].emplace_back(transform * submesh.transform);

            DrawCommand& drawCommand = physicsCollidersDrawCommandQueue[mk];
            drawCommand.vao = mesh.getVAO();
            drawCommand.material = physicsCollidersMaterial;
            drawCommand.baseIndex = submesh.baseIndex;
            drawCommand.baseVertex = submesh.baseVertex;
            drawCommand.indexCount = submesh.indexCount;
            drawCommand.instanceCount++;
        }
    }

    void SceneRenderer::shadowMapPass() {
        if (!currentSceneEnvironment.dirLightCastShadows) {
            clearPass(*shadowMapRenderPass);
            return;
        }

        Renderer::beginRenderPass(*shadowMapRenderPass);

        for (const auto [mk, command]: shadowMapDrawCommandQueue) {
            const auto& transforms = meshTransforms[mk];
            Renderer::executeDrawCommand(*command.vao, *shadowMapMaterial, command.indexCount, command.baseIndex, command.baseVertex, transforms, command.instanceCount);
        }

        Renderer::endRenderPass();
    }

    void SceneRenderer::geometryPass() {
        Renderer::beginRenderPass(*geometryRenderPass);

        geometryRenderPass->getSpecification().shader->setTextureCube(currentSceneEnvironment.irradianceMapId, 4);
        geometryRenderPass->getSpecification().shader->setTextureCube(currentSceneEnvironment.prefilterMapId, 5);
        geometryRenderPass->getSpecification().shader->setTexture2D(Renderer::getBrdfLUTTexture().getRendererId(), 6);
        geometryRenderPass->getSpecification().shader->setFloat("u_EnvironmentIntensity", currentSceneEnvironment.environmentIntensity);
        geometryRenderPass->getSpecification().shader->setTexture2D(shadowMapRenderPass->getSpecification().framebuffer->getDepthAttachmentRendererId(), 7);

        for (const auto [mk, command]: drawCommandQueue) {
            const auto& transforms = meshTransforms[mk];
            Renderer::executeDrawCommand(*command.vao, *command.material, command.indexCount, command.baseIndex, command.baseVertex, transforms, command.instanceCount);
        }

        Renderer::endRenderPass();
    }

    void SceneRenderer::skyboxPass() {
        Renderer::beginRenderPass(*skyboxRenderPass);
        Renderer::renderUnitCube(*skyboxMaterial);
        Renderer::endRenderPass();
    }

    void SceneRenderer::physicsCollidersPass() {
        Renderer::beginRenderPass(*physicsCollidersRenderPass);

        for (const auto [mk, command]: physicsCollidersDrawCommandQueue) {
            const auto& transforms = physicsCollidersMeshTransforms[mk];
            Renderer::executeDrawCommand(*command.vao, *command.material, command.indexCount, command.baseIndex, command.baseVertex, transforms, command.instanceCount);
        }

        Renderer::endRenderPass();
    }

    void SceneRenderer::normalsDebugPass() {
        Renderer::beginRenderPass(*normalsDebugRenderPass);

        for (const auto [mk, command]: drawCommandQueue) {
            const auto& transforms = meshTransforms[mk];
            Renderer::executeDrawCommand(*command.vao, *normalsDebugMaterial, command.indexCount, command.baseIndex, command.baseVertex, transforms, command.instanceCount);
        }

        Renderer::endRenderPass();
    }

    void SceneRenderer::screenPass() {
        Renderer::beginRenderPass(*screenRenderPass);
        Renderer::renderFullScreenQuad(*screenMaterial);
        Renderer::endRenderPass();
    }

    void SceneRenderer::clearPass(RenderPass &renderPass) {
        Renderer::beginRenderPass(renderPass);
        Renderer::endRenderPass();
    }

    void SceneRenderer::setupShadowMapData(glm::vec3 dirLightDirection) {
        UBDirShadowData dirShadowData{};

        float near_plane = 1.0f, far_plane = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(dirLightDirection * 50.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        dirShadowData.lightSpaceMat[0] = lightProjection * lightView;

        ubDirShadowData->setData(dirShadowData);
    }
}
