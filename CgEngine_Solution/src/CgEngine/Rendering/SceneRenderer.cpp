#include "SceneRenderer.h"
#include "Asserts.h"
#include "GlobalObjectManager.h"
#include "Renderer.h"
#include "Application.h"

namespace CgEngine {
    SceneRenderer::SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight) : viewportWidth(viewportWidth), viewportHeight(viewportHeight) {
        {
            ApplicationOptions& applicationOptions = Application::get().getApplicationOptions();

            dirShadowMaps = new Texture2DArray(TextureFormat::Depth, applicationOptions.shadowMapResolution, applicationOptions.shadowMapResolution, TextureWrap::ClampBorder, 4, MipMapFiltering::Bilinear);
            dirShadowMaps->setClampBorderColor({1.0f, 1.0f, 1.0f, 1.0f});

            FramebufferSpecification shadowMapFramebufferSpec;
            shadowMapFramebufferSpec.height = applicationOptions.shadowMapResolution;
            shadowMapFramebufferSpec.width = applicationOptions.shadowMapResolution;
            shadowMapFramebufferSpec.clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
            shadowMapFramebufferSpec.hasDepthStencilAttachment = false;
            shadowMapFramebufferSpec.hasDepthAttachment = false;
            shadowMapFramebufferSpec.useExistingDepthAttachment = true;
            shadowMapFramebufferSpec.existingDepthAttachment = dirShadowMaps->getRendererId();

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
//            bloomFilteredTexture = new Texture2D(TextureFormat::Float32, viewportWidth, viewportHeight, TextureWrap::Clamp, MipMapFiltering::Bilinear);
//            bloomFilteredTexture->generateMipMaps();
//
//            bloomDownsampleStagingTexture = new Texture2D(TextureFormat::Float32, viewportWidth, viewportHeight, TextureWrap::Clamp, MipMapFiltering::Bilinear);
//            bloomDownsampleStagingTexture->generateMipMaps();
//
//            bloomTexture = new Texture2D(TextureFormat::Float32, viewportWidth, viewportHeight, TextureWrap::Clamp, MipMapFiltering::Bilinear);
//            bloomTexture->generateMipMaps();
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
//            screenMaterial->setTexture2D("u_BloomTexture", *bloomFilteredTexture, 1);
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

        delete dirShadowMaps;
        delete bloomTexture;

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

        if (needsResize && viewportWidth != 0 && viewportHeight != 0) {
            needsResize = false;

            geometryRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);
            screenRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);

//            delete bloomFilteredTexture;
//            bloomFilteredTexture = new Texture2D(TextureFormat::Float32, viewportWidth, viewportHeight, TextureWrap::Clamp, MipMapFiltering::Bilinear);
//            bloomFilteredTexture->generateMipMaps();
//
//            delete bloomDownsampleStagingTexture;
//            bloomDownsampleStagingTexture = new Texture2D(TextureFormat::Float32, viewportWidth, viewportHeight, TextureWrap::Clamp, MipMapFiltering::Bilinear);
//            bloomDownsampleStagingTexture->generateMipMaps();
//
//            delete bloomTexture;
//            bloomTexture = new Texture2D(TextureFormat::Float32, viewportWidth, viewportHeight, TextureWrap::Clamp, MipMapFiltering::Bilinear);
//            bloomTexture->generateMipMaps();
//
//            screenMaterial->setTexture2D("u_FinalImage", geometryRenderPass->getSpecification().framebuffer->getColorAttachmentRendererId(0), 0);
//            screenMaterial->setTexture2D("u_BloomTexture", *bloomFilteredTexture, 1);
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

        setupShadowMapData(lightEnvironment.dirLightDirection, cameraData.viewProjection, camera);
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
//        bloomPass();
        screenPass();

        drawCommandQueue.clear();
        shadowMapDrawCommandQueue.clear();
        meshTransforms.clear();

        physicsCollidersDrawCommandQueue.clear();
        physicsCollidersMeshTransforms.clear();

        activeRendering = false;
    }

    void SceneRenderer::submitMesh(MeshVertices& mesh, const std::vector<uint32_t>& meshNodes, Material* overrideMaterial, bool castShadows, const glm::mat4& transform) {
        auto& submeshes = mesh.getSubmeshes();

        for (const auto& meshNodeIndex: meshNodes) {
            const auto& meshNode = mesh.getMeshNodes().at(meshNodeIndex);

            for (const auto& submeshIndex: meshNode.submeshIndices) {
                const Submesh& submesh = submeshes.at(submeshIndex);
                const Material* material = overrideMaterial != nullptr ? overrideMaterial : mesh.getMaterial(submesh.materialIndex);
                MeshKey mk = {mesh.getVAO()->getRendererId(), submeshIndex, material->getUuid().getUuid()};

                meshTransforms[mk].emplace_back(transform * meshNode.transform);

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
    }

    void SceneRenderer::submitPhysicsColliderMesh(MeshVertices& mesh, const glm::mat4& transform) {
        const auto& submeshes = mesh.getSubmeshes();

        for (const auto& meshNode: mesh.getMeshNodes()) {
            for (const auto& submeshIndex: meshNode.submeshIndices) {
                const Submesh& submesh = submeshes.at(submeshIndex);
                MeshKey mk = {mesh.getVAO()->getRendererId(), submeshIndex, physicsCollidersMaterial->getUuid().getUuid()};

                physicsCollidersMeshTransforms[mk].emplace_back(transform * meshNode.transform);

                DrawCommand& drawCommand = physicsCollidersDrawCommandQueue[mk];
                drawCommand.vao = mesh.getVAO();
                drawCommand.material = physicsCollidersMaterial;
                drawCommand.baseIndex = submesh.baseIndex;
                drawCommand.baseVertex = submesh.baseVertex;
                drawCommand.indexCount = submesh.indexCount;
                drawCommand.instanceCount++;
            }
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

        geometryRenderPass->getSpecification().shader->setTextureCube(currentSceneEnvironment.irradianceMapId, 5);
        geometryRenderPass->getSpecification().shader->setTextureCube(currentSceneEnvironment.prefilterMapId, 6);
        geometryRenderPass->getSpecification().shader->setTexture2D(Renderer::getBrdfLUTTexture().getRendererId(), 7);
        geometryRenderPass->getSpecification().shader->setFloat("u_EnvironmentIntensity", currentSceneEnvironment.environmentIntensity);
        geometryRenderPass->getSpecification().shader->setTexture2DArray(dirShadowMaps->getRendererId(), 8);

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

    void SceneRenderer::bloomPass() {
        auto& bloomShader = *GlobalObjectManager::getInstance().getResourceManager().getResource<ComputeShader>("bloom");
        bloomShader.bind();

        uint32_t mipWidth = bloomFilteredTexture->getWidth();
        uint32_t mipHeight = bloomFilteredTexture->getHeight();

        auto groupsX = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipWidth) / 4.0f));
        auto groupsY = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipHeight) / 4.0f));

        bloomShader.setInt("u_Mode", 0);

        bloomShader.setInt("u_LOD", 0);
        bloomShader.setFloat("u_Threshold", 1.0f);
        bloomShader.setFloat("u_Knee", 0.1f);
        bloomShader.setTexture2D(geometryRenderPass->getSpecification().framebuffer->getColorAttachmentRendererId(0), 0);
        bloomShader.setTexture2D(geometryRenderPass->getSpecification().framebuffer->getColorAttachmentRendererId(0), 1);
        bloomShader.setImage2D(*bloomFilteredTexture, 2, ShaderStorageAccess::WriteOnly, 0);
        bloomShader.dispatch(groupsX, groupsY, 1);
        bloomShader.waitForMemoryBarrier();

        bloomShader.setInt("u_Mode", 1);
        uint32_t mipCount = TextureUtils::calculateMipCount(bloomFilteredTexture->getWidth(), bloomFilteredTexture->getHeight()) - 2;
        for (int i = 1; i < mipCount; i++) {
            mipWidth = bloomFilteredTexture->getWidthForMip(i);
            mipHeight = bloomFilteredTexture->getHeightForMip(i);
            groupsX = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipWidth) / 4.0f));
            groupsY = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipHeight) / 4.0f));

            bloomShader.setInt("u_LOD", i - 1);
            bloomShader.setTexture2D(*bloomFilteredTexture, 0);
            bloomShader.setImage2D(*bloomFilteredTexture, 2, ShaderStorageAccess::WriteOnly, i);

            bloomShader.dispatch(groupsX, groupsY, 1);
            bloomShader.waitForMemoryBarrier();


//            bloomShader.setInt("u_LOD", i);
//            bloomShader.setTexture2D(*bloomDownsampleStagingTexture, 0);
//            bloomShader.setImage2D(*bloomFilteredTexture, 2, ShaderStorageAccess::WriteOnly, i);
//
//            bloomShader.dispatch(groupsX, groupsY, 1);
//            bloomShader.waitForMemoryBarrier();
        }

//        auto& bloomUpsampleShader = *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("upsample");
//        bloomUpsampleShader.bind();
//
//        uint32_t mFBO;
//        glGenFramebuffers(1, &mFBO);
//        glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
//
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_ONE, GL_ONE);
//        glBlendEquation(GL_FUNC_ADD);
//
//        for (int i = mipCount - 1; i > 0; i--) {
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomFilteredTexture->getRendererId(), i - 1);
//            glViewport(0, 0, bloomFilteredTexture->getWidthForMip(i - 1), bloomFilteredTexture->getHeightForMip(i - 1));
//
//            bloomUpsampleShader.setInt("u_LOD", i);
//            bloomFilteredTexture->bind(0);
//
//            Renderer::rendererData.quadVAO->bind();
//            glDrawElements(GL_TRIANGLES, Renderer::rendererData.quadVAO->getIndexCount(), GL_UNSIGNED_INT, nullptr);
//        }
//
//        glDisable(GL_BLEND);
//        glDeleteFramebuffers(1, &mFBO);
//
//
//        bloomShader.setInt("u_Mode", 2);
//        mipWidth = bloomFilteredTexture->getWidthForMip(mipCount - 2);
//        mipHeight = bloomFilteredTexture->getHeightForMip(mipCount - 2);
//        groupsX = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipWidth) / 4.0f));
//        groupsY = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipHeight) / 4.0f));
//
//        bloomShader.setInt("u_LOD", mipCount - 2);
//        bloomShader.setTexture2D(*bloomFilteredTexture, 0);
//        bloomShader.setImage2D(*bloomTexture, 2, ShaderStorageAccess::WriteOnly, mipCount - 2);
//
//        bloomShader.dispatch(groupsX, groupsY, 1);
//        bloomShader.waitForMemoryBarrier();
//
//        bloomShader.setInt("u_Mode", 3);
//        for (int i = mipCount - 3; i >= 0; i--) {
//            mipWidth = bloomFilteredTexture->getWidthForMip(i);
//            mipHeight = bloomFilteredTexture->getHeightForMip(i);
//            groupsX = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipWidth) / 4.0f));
//            groupsY = static_cast<uint32_t>(glm::ceil(static_cast<float>(mipHeight) / 4.0f));
//
//            bloomShader.setInt("u_LOD", i);
//            bloomShader.setTexture2D(*bloomFilteredTexture, 0);
//            bloomShader.setTexture2D(*bloomTexture, 1);
//            bloomShader.setImage2D(*bloomTexture, 2, ShaderStorageAccess::WriteOnly, i);
//
//            bloomShader.dispatch(groupsX, groupsY, 1);
//            bloomShader.waitForMemoryBarrier();
//        }

        for (int i = 0; i < bloomFramebuffers.size(); i++) {
            bloomFramebuffers[i]->bind();
        }
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

    void SceneRenderer::setupShadowMapData(glm::vec3 dirLightDirection, const glm::mat4& cameraViewProjection, const Camera& camera) {
        glm::vec4 cascadeSplits = {0.02f, 0.05f, 0.15f, 0.4f};
        glm::mat4 invCamViewProj = glm::inverse(cameraViewProjection);

        // https://stackoverflow.com/questions/33499053/cascaded-shadow-map-shimmering
        // https://learnopengl.com/Guest-Articles/2021/CSM

        UBDirShadowData dirShadowData{};

        float nearToFarPlane = camera.getPerspectiveFar() - camera.getPerspectiveNear();
        float lastSplitDist = 0.0;

        for (int i = 0; i < 4; i++) {
            glm::vec3 frustumCorners[8] = {
                    glm::vec3(-1.0f,  1.0f, -1.0f),
                    glm::vec3( 1.0f,  1.0f, -1.0f),
                    glm::vec3( 1.0f, -1.0f, -1.0f),
                    glm::vec3(-1.0f, -1.0f, -1.0f),
                    glm::vec3(-1.0f,  1.0f,  1.0f),
                    glm::vec3( 1.0f,  1.0f,  1.0f),
                    glm::vec3( 1.0f, -1.0f,  1.0f),
                    glm::vec3(-1.0f, -1.0f,  1.0f)
            };

            for (auto& frustumCorner : frustumCorners) {
                glm::vec4 invFrustumCorner = invCamViewProj * glm::vec4(frustumCorner, 1.0f);
                frustumCorner = invFrustumCorner / invFrustumCorner.w;
            }

            float splitDist = cascadeSplits[i];

            for (uint32_t j = 0; j < 4; j++) {
                glm::vec3 dist = frustumCorners[j + 4] - frustumCorners[j];
                frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
                frustumCorners[j] = frustumCorners[j] + (dist * lastSplitDist);
            }
            lastSplitDist = cascadeSplits[i];
            dirShadowData.cascadeSplits[i] = camera.getPerspectiveNear() + splitDist * nearToFarPlane;

            auto frustumCenter = glm::vec3(0.0f);
            for (auto& frustumCorner : frustumCorners) {
                frustumCenter += frustumCorner;
            }
            frustumCenter /= 8.0f;

            const auto lightView = glm::lookAt(frustumCenter + dirLightDirection, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

            float radius = 0.0f;
            for (auto& frustumCorner : frustumCorners) {
                float distance = glm::length(frustumCorner - frustumCenter);
                radius = glm::max(radius, distance);
            }
            radius = glm::ceil(radius);

            auto maxOrtho = glm::vec3(radius);
            glm::vec3 minOrtho = -maxOrtho;

            glm::mat4 lightProjection = glm::ortho(minOrtho.x, maxOrtho.x, minOrtho.y, maxOrtho.y, -50.0f, maxOrtho.z - minOrtho.z + 50.0f);

            glm::mat4 shadowMatrix = lightProjection * lightView;
            float shadowMapResolution = static_cast<float>(dirShadowMaps->getWidth());
            glm::vec4 shadowOrigin = (shadowMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) * shadowMapResolution / 2.0f;
            glm::vec4 roundedOrigin = glm::round(shadowOrigin);
            glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
            roundOffset = roundOffset * 2.0f / shadowMapResolution;
            roundOffset.z = 0.0f;
            roundOffset.w = 0.0f;

            lightProjection[3] += roundOffset;

            dirShadowData.lightSpaceMat[i] = lightProjection * lightView;
        }

        ubDirShadowData->setData(dirShadowData);
    }
}
