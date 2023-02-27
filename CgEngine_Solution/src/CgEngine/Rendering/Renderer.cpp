#include "Renderer.h"
#include "glad/glad.h"
#include "Asserts.h"

namespace CgEngine {
    RenderPass* Renderer::currentRenderPass = nullptr;
    RendererData* Renderer::rendererData;
    bool Renderer::isWireframe;
    bool Renderer::isBackfaceCulling;
    DepthCompareOperator Renderer::depthCompareOperator;
    bool Renderer::depthTest;
    bool Renderer::depthWrite;
    Texture2D* Renderer::whiteTexture;

    void Renderer::init() {
        rendererData = new RendererData();

        struct QuadVertex {
            glm::vec3 pos;
            glm::vec2 uv;
        };
        QuadVertex data[4];
        data[0].pos = {-1.0f, -1.0f, 0.0f};
        data[0].uv = {0.0f, 0.0f};

        data[1].pos = {1.0f, -1.0f, 0.0f};
        data[1].uv = {1.0f, 0.0f};

        data[2].pos = {1.0f, 1.0f, 0.0f};
        data[2].uv = {1.0f, 1.0f};

        data[3].pos = {-1.0f, 1.0f, 0.0f};
        data[3].uv = {0.0f, 1.0f};

        rendererData->quadVAO = new VertexArrayObject();
        auto quadVertexBuffer = std::make_shared<VertexBuffer>(data, 4 * sizeof(QuadVertex));
        quadVertexBuffer->setLayout({{ShaderDataType::Float3, true}, {ShaderDataType::Float2, true}});
        rendererData->quadVAO->addVertexBuffer(quadVertexBuffer);
        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
        rendererData->quadVAO->setIndexBuffer(indices, 6);

        isWireframe = false;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        isBackfaceCulling = true;
        glEnable(GL_CULL_FACE);
        depthTest = true;
        glEnable(GL_DEPTH_TEST);
        depthWrite = true;
        glDepthMask(GL_TRUE);
        depthCompareOperator = DepthCompareOperator::Less;
        glDepthFunc(static_cast<GLint>(depthCompareOperator));

        uint32_t whiteTextureData = 0xffffffff;
        whiteTexture = new Texture2D(TextureFormat::RGBA, 1, 1, TextureWrap::Clamp, &whiteTextureData);
    }

    void Renderer::shutdown() {
        delete rendererData;
    }

    void Renderer::beginRenderPass(RenderPass& renderPass) {
        CG_ASSERT(currentRenderPass == nullptr, "There already is an active RenderPass!")

        currentRenderPass = &renderPass;
        RenderPassSpecification& spec = renderPass.getSpecification();

        spec.shader->bind();
        spec.framebuffer->bind();

        if (spec.clearColorBuffer) {
            glm::vec4& clearColor = spec.framebuffer->getSpecification().clearColor;
            glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        if (spec.clearDepthBuffer) {
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        if (spec.clearStencilBuffer) {
            glClear(GL_STENCIL_BUFFER_BIT);
        }

        if (isWireframe != spec.wireframe) {
            isWireframe = spec.wireframe;
            if (isWireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        if (isBackfaceCulling != spec.backfaceCulling) {
            isBackfaceCulling = spec.backfaceCulling;
            if (isBackfaceCulling) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }
        }
        if (depthTest != spec.depthTest) {
            depthTest = spec.depthTest;
            if (depthTest) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
        }
        if (depthWrite != spec.depthWrite) {
            depthWrite = spec.depthWrite;
            if (depthWrite) {
                glDepthMask(GL_TRUE);
            } else {
                glDepthMask(GL_FALSE);
            }
        }
        if (depthCompareOperator != spec.depthCompareOperator) {
            depthCompareOperator = spec.depthCompareOperator;
            glDepthFunc(static_cast<GLint>(depthCompareOperator));
        }
    }

    void Renderer::endRenderPass() {
        CG_ASSERT(currentRenderPass != nullptr, "There is no active RenderPass!")

        currentRenderPass = nullptr;
    }

    void Renderer::renderFullScreenQuad(Material &material) {
        CG_ASSERT(currentRenderPass != nullptr, "There is no active RenderPass!")

        material.uploadToShader(*currentRenderPass->getSpecification().shader);

        rendererData->quadVAO->bind();
        glDrawElements(GL_TRIANGLES, rendererData->quadVAO->getIndexCount(), GL_UNSIGNED_INT, nullptr);
    }

    Texture2D& Renderer::getWhiteTexture() {
        return *whiteTexture;
    }
}
