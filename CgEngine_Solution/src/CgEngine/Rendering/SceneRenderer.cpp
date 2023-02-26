#include "SceneRenderer.h"
#include "Asserts.h"
#include "GlobalObjectManager.h"
#include "Renderer.h"

namespace CgEngine {
    SceneRenderer::SceneRenderer(uint32_t viewportWidth, uint32_t viewportHeight) : viewportWidth(viewportWidth), viewportHeight(viewportHeight) {
        {
            FramebufferSpecification geoFramebufferSpec;
            geoFramebufferSpec.height = viewportHeight;
            geoFramebufferSpec.width = viewportHeight;
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
            FramebufferSpecification screenFramebufferSpec;
            screenFramebufferSpec.height = viewportHeight;
            screenFramebufferSpec.width = viewportHeight;
            screenFramebufferSpec.clearColor = {0.0f, 1.0f, 1.0f, 1.0f};
            screenFramebufferSpec.colorAttachments = {FramebufferFormat::RGBA8};
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

        ubCamera = new UniformBuffer<UBCamera>("CameraData", 0, *GlobalObjectManager::getInstance().getResourceManager().getResource<Shader>("pbr"));
    }

    SceneRenderer::~SceneRenderer() {
        delete geometryRenderPass;
        delete screenRenderPass;
        delete screenMaterial;

        delete ubCamera;
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

    void SceneRenderer::beginScene(const Camera& camera, glm::mat4 cameraTransform) {
        CG_ASSERT(!activeRendering, "Already Rendering Scene!")
        CG_ASSERT(activeScene, "No active Scene!")

        activeRendering = true;

        if (needsResize) {
            needsResize = false;

            geometryRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);
            screenRenderPass->getSpecification().framebuffer->resize(viewportWidth, viewportHeight, false);
        }

        UBCamera cameraData{};
        cameraData.projection = camera.getProjectionMatrix();
        cameraData.view = glm::inverse(cameraTransform);
        cameraData.viewProjection = cameraData.projection * cameraData.view;
        cameraData.position = cameraTransform[3];

        ubCamera->setData(cameraData);
    }

    void SceneRenderer::endScene() {
        CG_ASSERT(activeRendering, "Not actively rendering!")

        geometryPass();
        screenPass();

        activeRendering = false;
    }

    void SceneRenderer::submitMesh(VertexArrayObject& vao, const Material& material, const glm::mat4& transform) {
        geoQueue.push_back({vao, material, transform});
    }

    void SceneRenderer::geometryPass() {
        Renderer::beginRenderPass(*geometryRenderPass);

        for (const auto &item: geoQueue) {
            item.material.uploadToShader(*geometryRenderPass->getSpecification().shader);
            geometryRenderPass->getSpecification().shader->setMat4("u_model", item.transform);
            item.vao.bind();
            glDrawElements(GL_TRIANGLES, item.vao.getIndexCount(), GL_UNSIGNED_INT, nullptr);
        }
        geoQueue.clear();

        Renderer::endRenderPass(*geometryRenderPass);
    }

    void SceneRenderer::screenPass() {
        Renderer::beginRenderPass(*screenRenderPass);
        Renderer::renderFullScreenQuad(*screenRenderPass, *screenMaterial);
        Renderer::endRenderPass(*screenRenderPass);
    }

    void SceneRenderer::clearPass(RenderPass &renderPass) {
        Renderer::beginRenderPass(renderPass);
        Renderer::endRenderPass(renderPass);
    }
}
