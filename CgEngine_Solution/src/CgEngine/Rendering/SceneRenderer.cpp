#include "SceneRenderer.h"
#include "Asserts.h"
#include "GlobalObjectManager.h"
#include "Application.h"

namespace CgEngine {
    SceneRenderer::SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight) : viewportWidth(viewportWidth), viewportHeight(viewportHeight) {
        emptyMaterial = new Material("EmptyMaterial");

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
        }
        {
            FramebufferSpecification preDepthFramebufferSpec;
            preDepthFramebufferSpec.height = viewportHeight;
            preDepthFramebufferSpec.width = viewportWidth;
            preDepthFramebufferSpec.clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
            preDepthFramebufferSpec.hasDepthStencilAttachment = false;
            preDepthFramebufferSpec.hasDepthAttachment = true;

            auto* framebuffer = new Framebuffer(preDepthFramebufferSpec);

            RenderPassSpecification preDepthRenderPassSpec;
            preDepthRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("preDepth");
            preDepthRenderPassSpec.framebuffer = framebuffer;
            preDepthRenderPassSpec.clearColorBuffer = false;
            preDepthRenderPassSpec.clearDepthBuffer = true;
            preDepthRenderPassSpec.depthCompareOperator = DepthCompareOperator::Less;

            preDepthRenderPass = new RenderPass(preDepthRenderPassSpec);
        }
        {
            FramebufferSpecification geoFramebufferSpec;
            geoFramebufferSpec.height = viewportHeight;
            geoFramebufferSpec.width = viewportWidth;
            geoFramebufferSpec.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            geoFramebufferSpec.colorAttachments = {FramebufferFormat::RGBA16F};
            geoFramebufferSpec.hasDepthStencilAttachment = false;
            geoFramebufferSpec.useExistingDepthAttachment = true;
            geoFramebufferSpec.existingDepthAttachment = preDepthRenderPass->getSpecification().framebuffer->getDepthAttachmentRendererId();
            geoFramebufferSpec.samples = 1;

            auto* framebuffer = new Framebuffer(geoFramebufferSpec);

            RenderPassSpecification geoRenderPassSpec;
            geoRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("pbr");
            geoRenderPassSpec.framebuffer = framebuffer;
            geoRenderPassSpec.depthCompareOperator = DepthCompareOperator::LessOrEqual;
            geoRenderPassSpec.clearDepthBuffer = false;
            geoRenderPassSpec.depthWrite = false;

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
            float bloomWidth = static_cast<float>(viewportWidth) / 2.0f;
            float bloomHeight = static_cast<float>(viewportHeight) / 2.0f;
            for (auto& bloomTexture : bloomTextures) {
                bloomTexture = new Texture2D(TextureFormat::Float32, static_cast<uint32_t>(bloomWidth), static_cast<uint32_t>(bloomHeight), TextureWrap::Clamp, MipMapFiltering::Bilinear);
                bloomWidth /= 2.0f;
                bloomHeight /= 2.0f;
            }

            FramebufferSpecification bloomFramebufferSpec;
            bloomFramebufferSpec.width = viewportWidth / 2;
            bloomFramebufferSpec.height = viewportHeight / 2;
            bloomFramebufferSpec.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            bloomFramebufferSpec.hasDepthStencilAttachment = false;
            bloomFramebufferSpec.useExistingColorAttachment = true;
            bloomFramebufferSpec.existingColorAttachment = bloomTextures[0]->getRendererId();
            bloomFramebufferSpec.existingColorAttachmentLevel = 0;

            auto* framebuffer = new Framebuffer(bloomFramebufferSpec);

            RenderPassSpecification bloomDownSamplePassSpec;
            bloomDownSamplePassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("bloomDownSample");
            bloomDownSamplePassSpec.clearColorBuffer = true;
            bloomDownSamplePassSpec.clearDepthBuffer = false;
            bloomDownSamplePassSpec.depthTest = false;
            bloomDownSamplePassSpec.depthWrite = false;
            bloomDownSamplePassSpec.framebuffer = framebuffer;

            bloomDownSamplePass = new RenderPass(bloomDownSamplePassSpec);

            RenderPassSpecification bloomUpSamplePassSpec;
            bloomUpSamplePassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("bloomUpSample");
            bloomUpSamplePassSpec.clearColorBuffer = false;
            bloomUpSamplePassSpec.clearDepthBuffer = false;
            bloomUpSamplePassSpec.depthTest = false;
            bloomUpSamplePassSpec.depthWrite = false;
            bloomUpSamplePassSpec.useBlending = true;
            bloomUpSamplePassSpec.blendingEquation = BlendingEquation::Add;
            bloomUpSamplePassSpec.srcBlendingFunction = BlendingFunction::One;
            bloomUpSamplePassSpec.destBlendingFunction = BlendingFunction::One;
            bloomUpSamplePassSpec.framebuffer = framebuffer;

            bloomUpSamplePass = new RenderPass(bloomUpSamplePassSpec);
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
            RenderPassSpecification debugLinesRenderPassSpec;
            debugLinesRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("lines");
            debugLinesRenderPassSpec.framebuffer = geometryRenderPass->getSpecification().framebuffer;
            debugLinesRenderPassSpec.depthTest = true;
            debugLinesRenderPassSpec.depthWrite = false;
            debugLinesRenderPassSpec.clearColorBuffer = false;
            debugLinesRenderPassSpec.clearDepthBuffer = false;
            debugLinesRenderPassSpec.clearStencilBuffer = false;

            debugLinesRenderPass = new RenderPass(debugLinesRenderPassSpec);
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
            screenMaterial->setTexture("u_FinalImage", geometryRenderPass->getSpecification().framebuffer->getColorAttachmentRendererId(0), 0);
            screenMaterial->setTexture("u_BloomTexture", bloomTextures[0]->getRendererId(), 1);
        }
        {
            RenderPassSpecification uiCircleRenderPassSpec;
            uiCircleRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("uiCircle");
            uiCircleRenderPassSpec.clearColorBuffer = false;
            uiCircleRenderPassSpec.clearDepthBuffer = false;
            uiCircleRenderPassSpec.depthTest = false;
            uiCircleRenderPassSpec.depthWrite = false;
            uiCircleRenderPassSpec.useBlending = true;
            uiCircleRenderPassSpec.blendingEquation = BlendingEquation::Add;
            uiCircleRenderPassSpec.srcBlendingFunction = BlendingFunction::SrcAlpha;
            uiCircleRenderPassSpec.destBlendingFunction = BlendingFunction::OneMinusSrcAlpha;
            uiCircleRenderPassSpec.framebuffer = screenRenderPass->getSpecification().framebuffer;

            uiCirclePass = new RenderPass(uiCircleRenderPassSpec);

            RenderPassSpecification uiRectRenderPassSpec;
            uiRectRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("uiRect");
            uiRectRenderPassSpec.clearColorBuffer = false;
            uiRectRenderPassSpec.clearDepthBuffer = false;
            uiRectRenderPassSpec.depthTest = false;
            uiRectRenderPassSpec.depthWrite = false;
            uiRectRenderPassSpec.useBlending = true;
            uiRectRenderPassSpec.blendingEquation = BlendingEquation::Add;
            uiRectRenderPassSpec.srcBlendingFunction = BlendingFunction::SrcAlpha;
            uiRectRenderPassSpec.destBlendingFunction = BlendingFunction::OneMinusSrcAlpha;
            uiRectRenderPassSpec.framebuffer = screenRenderPass->getSpecification().framebuffer;

            uiRectPass = new RenderPass(uiRectRenderPassSpec);

            RenderPassSpecification uiTextRenderPassSpec;
            uiTextRenderPassSpec.shader = GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("uiText");
            uiTextRenderPassSpec.clearColorBuffer = false;
            uiTextRenderPassSpec.clearDepthBuffer = false;
            uiTextRenderPassSpec.depthTest = false;
            uiTextRenderPassSpec.depthWrite = false;
            uiTextRenderPassSpec.useBlending = true;
            uiTextRenderPassSpec.blendingEquation = BlendingEquation::Add;
            uiTextRenderPassSpec.srcBlendingFunction = BlendingFunction::SrcAlpha;
            uiTextRenderPassSpec.destBlendingFunction = BlendingFunction::OneMinusSrcAlpha;
            uiTextRenderPassSpec.framebuffer = screenRenderPass->getSpecification().framebuffer;

            uiTextPass = new RenderPass(uiTextRenderPassSpec);

            uiProjectionMatrix = glm::ortho(0.0f, static_cast<float>(viewportWidth), 0.0f, static_cast<float>(viewportHeight));
        }

        ubCameraData = new UniformBuffer<UBCameraData>("CameraData", 0, *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("pbr"));
        ubLightData = new UniformBuffer<UBLightData>("LightData", 1, *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("pbr"));
        ubDirShadowData = new UniformBuffer<UBDirShadowData>("DirShadowData", 2, *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("dirShadowMap"));

        boneTransformsBuffer = new ShaderStorageBuffer();
    }

    SceneRenderer::~SceneRenderer() {
        delete shadowMapRenderPass;
        delete preDepthRenderPass;
        delete geometryRenderPass;
        delete skyboxRenderPass;
        delete bloomDownSamplePass;
        delete bloomUpSamplePass;
        delete uiCirclePass;
        delete uiRectPass;
        delete uiTextPass;
        delete physicsCollidersRenderPass;
        delete normalsDebugRenderPass;
        delete debugLinesRenderPass;
        delete screenRenderPass;

        delete skyboxMaterial;
        delete physicsCollidersMaterial;
        delete normalsDebugMaterial;
        delete screenMaterial;
        delete emptyMaterial;

        delete dirShadowMaps;

        for (int i = 0; i < bloomTextures.size(); i++) {
            delete bloomTextures[i];
        }

        delete ubCameraData;
        delete ubLightData;
        delete ubDirShadowData;

        delete boneTransformsBuffer;
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

            preDepthRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);
            geometryRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);
            geometryRenderPass->getSpecification().framebuffer->setDepthAttachment(preDepthRenderPass->getSpecification().framebuffer->getDepthAttachmentRendererId(), 0, viewportWidth, viewportHeight);
            screenRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);

            float bloomWidth = static_cast<float>(viewportWidth) / 2.0f;
            float bloomHeight = static_cast<float>(viewportHeight) / 2.0f;
            for (int i = 0; i < bloomTextures.size(); i++) {
                delete bloomTextures[i];
                bloomTextures[i] = new Texture2D(TextureFormat::Float32, static_cast<uint32_t>(bloomWidth), static_cast<uint32_t>(bloomHeight), TextureWrap::Clamp, MipMapFiltering::Bilinear);
                bloomWidth /= 2.0f;
                bloomHeight /= 2.0f;
            }

            screenMaterial->setTexture("u_FinalImage", geometryRenderPass->getSpecification().framebuffer->getColorAttachmentRendererId(0), 0);
            screenMaterial->setTexture("u_BloomTexture", bloomTextures[0]->getRendererId(), 1);

            uiProjectionMatrix = glm::ortho(0.0f, static_cast<float>(viewportWidth), 0.0f, static_cast<float>(viewportHeight));
        }

        UBCameraData cameraData{};
        cameraData.projection = camera.getProjectionMatrix();
        cameraData.view = glm::inverse(cameraTransform);
        cameraData.viewProjection = cameraData.projection * cameraData.view;
        cameraData.uiProjectionMatrix = uiProjectionMatrix;
        cameraData.position = cameraTransform[3];
        cameraData.exposure = camera.getExposure();
        cameraData.bloomIntensity = camera.getBloomIntensity();
        cameraData.bloomThreshold = camera.getBloomThreshold();
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

        skyboxMaterial->setTextureCube("u_Texture", *sceneEnvironment.prefilterMap, 0);
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
        preDepthPass();
        geometryPass();
        skyboxPass();
        if (applicationOptions.debugShowPhysicsColliders) {
            physicsCollidersPass();
        }
        if (applicationOptions.debugShowNormals) {
            normalsDebugPass();
        }
        if (applicationOptions.debugRenderLines) {
            debugLinesPass();
        }
        if (applicationOptions.enableBloom) {
            bloomPass();
        }
        screenPass();
        uiPass();

        drawCommandQueue.clear();
        shadowMapDrawCommandQueue.clear();
        meshTransforms.clear();

        uiDrawInfoQueue.clear();

        physicsCollidersDrawCommandQueue.clear();
        physicsCollidersMeshTransforms.clear();

        debugLinesDrawInfoQueue.clear();

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

    void SceneRenderer::submitAnimatedMesh(MeshVertices& mesh, const std::vector<uint32_t>& meshNodes, Material* overrideMaterial, bool castShadows, const glm::mat4& transform, const std::vector<glm::mat4>& boneTransforms, VertexArrayObject* skinnedVAO) {
        auto& resourceManager = GlobalObjectManager::getInstance().getResourceManager();
        auto& skinning = *resourceManager.getResource<ComputeShader>("skinning");

        boneTransformsBuffer->setData(boneTransforms.data(), boneTransforms.size() * sizeof(glm::mat4));
        boneTransformsBuffer->bind(2);
        mesh.getVAO()->getVertexBuffers()[0]->bindAsSSBO(3);
        skinnedVAO->getVertexBuffers()[0]->bindAsSSBO(4);
        mesh.getBoneInfluencesBuffer()->bind(1);

        skinning.bind();
        skinning.dispatch((mesh.getVertices().size() + (mesh.getVertices().size() % 32)) / 32, 1, 1);
        skinning.waitForMemoryBarrier();

        auto& submeshes = mesh.getSubmeshes();

        for (const auto& meshNodeIndex: meshNodes) {
            const auto& meshNode = mesh.getMeshNodes().at(meshNodeIndex);

            for (const auto& submeshIndex: meshNode.submeshIndices) {
                const Submesh& submesh = submeshes.at(submeshIndex);
                const Material* material = overrideMaterial != nullptr ? overrideMaterial : mesh.getMaterial(submesh.materialIndex);
                MeshKey mk = {skinnedVAO->getRendererId(), submeshIndex, material->getUuid().getUuid()};

                meshTransforms[mk].emplace_back(transform);

                DrawCommand& drawCommand = drawCommandQueue[mk];
                drawCommand.vao = skinnedVAO;
                drawCommand.material = material;
                drawCommand.baseIndex = submesh.baseIndex;
                drawCommand.baseVertex = submesh.baseVertex;
                drawCommand.indexCount = submesh.indexCount;
                drawCommand.instanceCount++;

                if (castShadows) {
                    DrawCommand& shadowMapDrawCommand = shadowMapDrawCommandQueue[mk];
                    shadowMapDrawCommand.vao = skinnedVAO;
                    shadowMapDrawCommand.material = material;
                    shadowMapDrawCommand.baseIndex = submesh.baseIndex;
                    shadowMapDrawCommand.baseVertex = submesh.baseVertex;
                    shadowMapDrawCommand.indexCount = submesh.indexCount;
                    shadowMapDrawCommand.instanceCount++;
                }
            }
        }
    }

    void SceneRenderer::submitUiElements(const std::unordered_map<std::string, UiElement*>& uiElements) {
        for (const auto& [_, element]: uiElements) {
            UiDrawInfo& drawInfo = uiDrawInfoQueue[element->getZIndex()];

            if (element->getType() == UIElementType::Circle) {
                auto* circleElement = dynamic_cast<UiCircle*>(element);
                const auto* texture = circleElement->getTexture();
                float textureIndex = findDrawInfoTextureIndex(drawInfo, texture);

                for (const auto& v: element->getVertices()) {
                    UiCircleVertex& vertex = drawInfo.circleVertices.emplace_back();
                    vertex.posUV = v;
                    vertex.fillColor = circleElement->getFillColor();
                    vertex.width = circleElement->getWidth();
                    vertex.lineColor = circleElement->getLineColor();
                    vertex.lineWidth = circleElement->getLineWidth();
                    vertex.textureIndex = textureIndex;
                }
                drawInfo.circleIndexCount += 6;
            } else if (element->getType() == UIElementType::Rect) {
                auto* rectElement = dynamic_cast<UiRect*>(element);
                const auto* texture = rectElement->getTexture();
                float textureIndex = findDrawInfoTextureIndex(drawInfo, texture);

                for (const auto& v: element->getVertices()) {
                    UiRectVertex& vertex = drawInfo.rectVertices.emplace_back();
                    vertex.posUV = v;
                    vertex.fillColor = rectElement->getFillColor();
                    vertex.lineColor = rectElement->getLineColor();
                    vertex.size = rectElement->getSize();
                    vertex.lineWidth = rectElement->getLineWidth();
                    vertex.textureIndex = textureIndex;
                }
                drawInfo.rectIndexCount += 6;
            } else if (element->getType() == UIElementType::Text) {
                auto* textElement = dynamic_cast<UiText*>(element);
                const auto* fontAtlas = textElement->getFontAtlas();

                float fontAtlasIndex = -1;
                for (uint32_t i = 0; i < drawInfo.filledFontAtlases; i++) {
                    if (*drawInfo.fontAtlases[i] == *fontAtlas) {
                        fontAtlasIndex = static_cast<float>(i);
                        break;
                    }
                }
                if (fontAtlasIndex < 0.0f) {
                    fontAtlasIndex = static_cast<float>(drawInfo.filledFontAtlases);
                    drawInfo.fontAtlases[drawInfo.filledFontAtlases] = fontAtlas;
                    drawInfo.filledFontAtlases++;
                }

                for (const auto& v: element->getVertices()) {
                    UiTextVertex& vertex = drawInfo.textVertices.emplace_back();
                    vertex.posUV = v;
                    vertex.color = textElement->getColor();
                    vertex.fontAtlasIndex = fontAtlasIndex;
                }

                drawInfo.textIndexCount += textElement->getNumIndices();
            }

            CG_ASSERT(drawInfo.circleIndexCount <= Renderer::maxUiIndices, "Cannot render that many UICircles")
            CG_ASSERT(drawInfo.rectIndexCount <= Renderer::maxUiIndices, "Cannot render that many UIRects")
            CG_ASSERT(drawInfo.textIndexCount <= Renderer::maxUiIndices, "Cannot render that many UIText")
            CG_ASSERT(drawInfo.filledTextureSlots < drawInfo.textureSlots.size(), "Cannot render that many different Textures on a single z-index")
            CG_ASSERT(drawInfo.filledFontAtlases < drawInfo.fontAtlases.size(), "Cannot render that many different Fonts on a single z-index")
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

    void SceneRenderer::submitDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color) {
        auto& lineInfo = debugLinesDrawInfoQueue.emplace_back();
        lineInfo.from = from;
        lineInfo.to = to;
        lineInfo.color = color;
    }

    void SceneRenderer::shadowMapPass() {
        if (!currentSceneEnvironment.dirLightCastShadows) {
            clearPass(*shadowMapRenderPass);
            return;
        }

        Renderer::beginRenderPass(*shadowMapRenderPass);

        for (const auto [mk, command]: shadowMapDrawCommandQueue) {
            const auto& transforms = meshTransforms[mk];
            Renderer::executeDrawCommand(*command.vao, *emptyMaterial, command.indexCount, command.baseIndex, command.baseVertex, transforms, command.instanceCount);
        }

        Renderer::endRenderPass();
    }

    void SceneRenderer::preDepthPass() {
        Renderer::beginRenderPass(*preDepthRenderPass);

        for (const auto [mk, command]: drawCommandQueue) {
            const auto& transforms = meshTransforms[mk];
            Renderer::executeDrawCommand(*command.vao, *emptyMaterial, command.indexCount, command.baseIndex, command.baseVertex, transforms, command.instanceCount);
        }

        Renderer::endRenderPass();
    }

    void SceneRenderer::geometryPass() {
        Renderer::beginRenderPass(*geometryRenderPass);

        geometryRenderPass->getSpecification().shader->setTexture(currentSceneEnvironment.irradianceMapId, 5);
        geometryRenderPass->getSpecification().shader->setTexture(currentSceneEnvironment.prefilterMapId, 6);
        geometryRenderPass->getSpecification().shader->setTexture(Renderer::getBrdfLUTTexture().getRendererId(), 7);
        geometryRenderPass->getSpecification().shader->setFloat("u_EnvironmentIntensity", currentSceneEnvironment.environmentIntensity);
        geometryRenderPass->getSpecification().shader->setTexture(dirShadowMaps->getRendererId(), 8);

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

    void SceneRenderer::debugLinesPass() {
        Renderer::beginRenderPass(*debugLinesRenderPass);
        Renderer::renderLines(debugLinesDrawInfoQueue);
        Renderer::endRenderPass();
    }

    void SceneRenderer::bloomPass() {
        auto& downSampleShader = *bloomDownSamplePass->getSpecification().shader;

        bloomDownSamplePass->getSpecification().framebuffer->setColorAttachment(bloomTextures[0]->getRendererId(), 0, viewportWidth / 2, viewportHeight / 2);
        Renderer::beginRenderPass(*bloomDownSamplePass);
        downSampleShader.setTexture(geometryRenderPass->getSpecification().framebuffer->getColorAttachmentRendererId(0), 0);
        downSampleShader.setBool("u_UseThreshold", true);
        Renderer::renderUnitQuad(*emptyMaterial);
        Renderer::endRenderPass();

        downSampleShader.setBool("u_UseThreshold", false);
        for (uint32_t i = 0; i < bloomTextures.size() - 1; ++i) {
            bloomDownSamplePass->getSpecification().framebuffer->setColorAttachment(bloomTextures[i + 1]->getRendererId(), 0, bloomTextures[i + 1]->getWidth(), bloomTextures[i + 1]->getHeight());
            Renderer::beginRenderPass(*bloomDownSamplePass);
            downSampleShader.setTexture(bloomTextures[i]->getRendererId(), 0);
            Renderer::renderUnitQuad(*emptyMaterial);
            Renderer::endRenderPass();
        }

        auto& upSampleShader = *bloomUpSamplePass->getSpecification().shader;

        for (uint32_t i = bloomTextures.size() - 1; i > 0; i--) {
            bloomUpSamplePass->getSpecification().framebuffer->setColorAttachment(bloomTextures[i - 1]->getRendererId(), 0, bloomTextures[i - 1]->getWidth(), bloomTextures[i - 1]->getHeight());
            Renderer::beginRenderPass(*bloomUpSamplePass);
            upSampleShader.setTexture(bloomTextures[i]->getRendererId(), 0);
            Renderer::renderUnitQuad(*emptyMaterial);
            Renderer::endRenderPass();
        }
    }

    void SceneRenderer::screenPass() {
        Renderer::beginRenderPass(*screenRenderPass);
        Renderer::renderUnitQuad(*screenMaterial);
        Renderer::endRenderPass();
    }

    void SceneRenderer::uiPass() {
        for (const auto& [zIndex, drawInfo]: uiDrawInfoQueue) {

            for (uint32_t i = 0; i < drawInfo.filledTextureSlots; i++) {
                drawInfo.textureSlots[i]->bind(i);
            }
            for (uint32_t i = drawInfo.filledTextureSlots; i < drawInfo.textureSlots.size(); i++) {
                Renderer::getWhiteTexture().bind(i);
            }
            if (drawInfo.circleIndexCount > 0) {
                Renderer::beginRenderPass(*uiCirclePass);
                Renderer::renderUiCircles(drawInfo.circleVertices, drawInfo.circleIndexCount);
                Renderer::endRenderPass();
            }
            if (drawInfo.rectIndexCount > 0) {
                Renderer::beginRenderPass(*uiRectPass);
                Renderer::renderUiRects(drawInfo.rectVertices, drawInfo.rectIndexCount);
                Renderer::endRenderPass();
            }


            for (uint32_t i = 0; i < drawInfo.filledFontAtlases; i++) {
                drawInfo.fontAtlases[i]->bind(i);
            }
            for (uint32_t i = drawInfo.filledFontAtlases; i < drawInfo.fontAtlases.size(); i++) {
                Renderer::getWhiteTexture().bind(i);
            }
            if (drawInfo.textIndexCount > 0) {
                Renderer::beginRenderPass(*uiTextPass);
                Renderer::renderUiText(drawInfo.textVertices, drawInfo.textIndexCount);
                Renderer::endRenderPass();
            }
        }
    }

    void SceneRenderer::clearPass(RenderPass& renderPass) {
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

    float SceneRenderer::findDrawInfoTextureIndex(UiDrawInfo& drawInfo, const Texture2D* texture) const {
        float textureIndex = -1;
        if (texture != nullptr) {
            for (uint32_t i = 0; i < drawInfo.filledTextureSlots; i++) {
                if (*drawInfo.textureSlots[i] == *texture) {
                    textureIndex = static_cast<float>(i);
                    break;
                }
            }
            if (textureIndex < 0.0f) {
                textureIndex = static_cast<float>(drawInfo.filledTextureSlots);
                drawInfo.textureSlots[drawInfo.filledTextureSlots] = texture;
                drawInfo.filledTextureSlots++;
            }
        }
        return textureIndex;
    }
}
