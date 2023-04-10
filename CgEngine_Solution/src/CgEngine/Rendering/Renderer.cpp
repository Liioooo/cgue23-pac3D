#include "Renderer.h"
#include "glad/glad.h"
#include "Asserts.h"
#include "GlobalObjectManager.h"
#include "FileSystem.h"

namespace CgEngine {
    RenderPass* Renderer::currentRenderPass = nullptr;
    RendererData Renderer::rendererData;
    bool Renderer::isWireframe;
    bool Renderer::isBackFaceCulling;
    bool Renderer::isFrontFaceCulling;
    DepthCompareOperator Renderer::depthCompareOperator;
    bool Renderer::depthTest;
    bool Renderer::depthWrite;
    Texture2D* Renderer::whiteTexture;
    Texture2D* Renderer::brdfLUT;
    TextureCube* Renderer::blackCubeTexture;
    ShaderStorageBuffer* Renderer::transformsBuffer;

    void Renderer::init() {
        rendererData = RendererData();

        struct QuadVertex {
            glm::vec3 pos;
            glm::vec2 uv;
        };
        QuadVertex quadVertexData[4];
        quadVertexData[0].pos = {-1.0f, -1.0f, 0.0f};
        quadVertexData[0].uv = {0.0f, 0.0f};

        quadVertexData[1].pos = {1.0f, -1.0f, 0.0f};
        quadVertexData[1].uv = {1.0f, 0.0f};

        quadVertexData[2].pos = {1.0f, 1.0f, 0.0f};
        quadVertexData[2].uv = {1.0f, 1.0f};

        quadVertexData[3].pos = {-1.0f, 1.0f, 0.0f};
        quadVertexData[3].uv = {0.0f, 1.0f};

        rendererData.quadVAO = std::make_unique<VertexArrayObject>();
        auto quadVertexBuffer = std::make_shared<VertexBuffer>(quadVertexData, 4 * sizeof(QuadVertex));
        quadVertexBuffer->setLayout({{ShaderDataType::Float3, true}, {ShaderDataType::Float2, true}});
        rendererData.quadVAO->addVertexBuffer(quadVertexBuffer);
        uint32_t quadIndices[6] = {0, 1, 2, 2, 3, 0 };
        rendererData.quadVAO->setIndexBuffer(quadIndices, 6);

        float unitCubeVertices[] = {
                -1.0f, 1.0f, 1.0f, // left_top_front_0
                -1.0f, -1.0f, 1.0f, // left_bottom_front_1
                1.0f, 1.0f, 1.0f, // right_top_front_2
                1.0f, -1.0f, 1.0f, // right_bottom_front_3
                1.0f, 1.0f, -1.0f, // right_top_back_4
                1.0f, -1.0f, -1.0f, // right_bottom_back_5
                -1.0f, 1.0f, -1.0f, // left_top_back_6
                -1.0f, -1.0f, -1.0f, // left_bottom_back_7
        };

        uint32_t unitCubeIndices[] = {
                7, 5, 4,
                4, 6, 7,
                3, 2, 4,
                4, 5, 3,
                1, 7, 0,
                0, 7, 6,
                0, 2, 3,
                3, 1, 0,
                6, 4, 0,
                0, 4, 2,
                1, 3, 7,
                5, 7, 3
        };

        rendererData.unitCubeVAO = std::make_unique<VertexArrayObject>();
        auto unitCubeVertexBuffer = std::make_shared<VertexBuffer>(unitCubeVertices, sizeof(unitCubeVertices));
        unitCubeVertexBuffer->setLayout({{ShaderDataType::Float3, false}});
        rendererData.unitCubeVAO->addVertexBuffer(unitCubeVertexBuffer);
        rendererData.unitCubeVAO->setIndexBuffer(unitCubeIndices, 36);


        isWireframe = false;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        isBackFaceCulling = true;
        isFrontFaceCulling = false;
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        depthTest = true;
        glEnable(GL_DEPTH_TEST);
        depthWrite = true;
        glDepthMask(GL_TRUE);
        depthCompareOperator = DepthCompareOperator::Less;
        glDepthFunc(static_cast<GLint>(depthCompareOperator));

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        uint32_t whiteTextureData = 0xffffffff;
        whiteTexture = new Texture2D(TextureFormat::RGBA, 1, 1, TextureWrap::Clamp, &whiteTextureData, MipMapFiltering::Nearest);

        uint32_t blackCubeMapTextureData = 0xff000000;
        blackCubeTexture = new TextureCube(TextureFormat::RGBA, 1, 1, &blackCubeMapTextureData, MipMapFiltering::Nearest);

        brdfLUT = new Texture2D(FileSystem::getAsEnginePath("ibl_brdf_lut.png"), true, TextureWrap::Clamp, MipMapFiltering::Bilinear);

        transformsBuffer = new ShaderStorageBuffer(0);
    }

    void Renderer::shutdown() {
        delete whiteTexture;
        delete blackCubeTexture;
        delete brdfLUT;
        delete transformsBuffer;
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
        if (isBackFaceCulling != spec.backfaceCulling || isFrontFaceCulling != spec.frontfaceCulling) {
            isBackFaceCulling = spec.backfaceCulling;
            isFrontFaceCulling = spec.frontfaceCulling;
            if (isBackFaceCulling || isFrontFaceCulling) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }
            if (isBackFaceCulling) {
                glCullFace(GL_BACK);
            }
            if (isFrontFaceCulling) {
                glCullFace(GL_FRONT);
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

    void Renderer::renderFullScreenQuad(const Material &material) {
        CG_ASSERT(currentRenderPass != nullptr, "There is no active RenderPass!")

        material.uploadToShader(*currentRenderPass->getSpecification().shader);

        rendererData.quadVAO->bind();
        glDrawElements(GL_TRIANGLES, rendererData.quadVAO->getIndexCount(), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::renderUnitCube(const Material &material) {
        CG_ASSERT(currentRenderPass != nullptr, "There is no active RenderPass!")

        material.uploadToShader(*currentRenderPass->getSpecification().shader);

        rendererData.unitCubeVAO->bind();
        glDrawElements(GL_TRIANGLES, rendererData.unitCubeVAO->getIndexCount(), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::executeDrawCommand(const VertexArrayObject& vao, const Material& material, uint32_t indexCount, uint32_t baseIndex, uint32_t baseVertex, const std::vector<glm::mat4>& transforms, uint32_t instanceCount) {
        CG_ASSERT(currentRenderPass != nullptr, "There is no active RenderPass!")

        material.uploadToShader(*currentRenderPass->getSpecification().shader);
        vao.bind();
        transformsBuffer->setData(transforms.data(), transforms.size() * sizeof(glm::mat4));
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(baseIndex * sizeof(uint32_t)), instanceCount, baseVertex);
    }

    Texture2D& Renderer::getWhiteTexture() {
        return *whiteTexture;
    }

    Texture2D &Renderer::getBrdfLUTTexture() {
        return *brdfLUT;
    }

    TextureCube& Renderer::getBlackCubeTexture() {
        return *blackCubeTexture;
    }

    Material* Renderer::getDefaultPBRMaterial() {
        auto* defaultPBRMaterial = new Material("defaultPBRMaterial");
        defaultPBRMaterial->set("u_Mat_AlbedoColor", {1.0f, 1.0f, 1.0f});
        defaultPBRMaterial->set("u_Mat_Metalness", 0.0f);
        defaultPBRMaterial->set("u_Mat_Roughness", 1.0f);
        defaultPBRMaterial->set("u_Mat_Emission", 0.0f);
        defaultPBRMaterial->setTexture2D("u_Mat_AlbedoTexture", Renderer::getWhiteTexture(), 0);
        defaultPBRMaterial->setTexture2D("u_Mat_MetalnessTexture", Renderer::getWhiteTexture(), 2);
        defaultPBRMaterial->setTexture2D("u_Mat_RoughnessTexture", Renderer::getWhiteTexture(), 3);
        defaultPBRMaterial->setTexture2D("u_Mat_NormalTexture", Renderer::getWhiteTexture(), 1);
        defaultPBRMaterial->set("u_Mat_UseNormals", false);
        return defaultPBRMaterial;
    }

    std::pair<TextureCube*, TextureCube*> Renderer::createEnvironmentMap(const std::string &hdriPath) {
        auto& resourceManager = GlobalObjectManager::getInstance().getResourceManager();

        TextureCube* irradianceMap = nullptr;
        TextureCube* prefilterMap = nullptr;
        if (resourceManager.hasResource<TextureCube>(hdriPath + "-irradiance")) {
            irradianceMap = resourceManager.getResource<TextureCube>(hdriPath + "-irradiance");
        }
        if (resourceManager.hasResource<TextureCube>(hdriPath + "-prefilter")) {
            prefilterMap = resourceManager.getResource<TextureCube>(hdriPath + "-prefilter");
        }

        if (irradianceMap != nullptr && prefilterMap != nullptr) {
            return {irradianceMap, prefilterMap};
        }

        const uint32_t MAP_SIZE = 1024;

        Texture2D sphereMap("assets/game/" + hdriPath, false);

        CG_ASSERT(sphereMap.isLoaded(), "HDRI could not be loaded!");

        TextureCube cubeMap(TextureFormat::Float32, MAP_SIZE, MAP_SIZE, MipMapFiltering::Bilinear);

        auto& sphereToCubeShader = *resourceManager.getResource<ComputeShader>("sphereToCube");
        sphereToCubeShader.bind();
        sphereToCubeShader.setTexture2D(sphereMap, 0);
        sphereToCubeShader.setImageCube(cubeMap, 1, ShaderStorageAccess::WriteOnly, 0);
        sphereToCubeShader.dispatch(MAP_SIZE / 32, MAP_SIZE / 32, 6);
        sphereToCubeShader.waitForMemoryBarrier();

        cubeMap.generateMipMaps();
        TextureUtils::applyMipMapFiltering(MipMapFiltering::Trilinear, GL_TEXTURE_CUBE_MAP);

        uint32_t mipCount = TextureUtils::calculateMipCount(MAP_SIZE, MAP_SIZE);

        prefilterMap = new TextureCube(TextureFormat::Float32, MAP_SIZE, MAP_SIZE, MipMapFiltering::Trilinear);
        prefilterMap->generateMipMaps();

        auto& prefilterMapShader = *resourceManager.getResource<ComputeShader>("prefilterMap");
        prefilterMapShader.bind();
        prefilterMapShader.setTextureCube(cubeMap, 0);

        for (uint32_t i = 0, size = MAP_SIZE; i < mipCount; i++, size /= 2) {
            uint32_t numGroups = glm::max(1u, size / 32);
            float roughness = static_cast<float>(i) / static_cast<float>(mipCount - 1);
            prefilterMapShader.setFloat("u_Roughness", roughness);
            prefilterMapShader.setImageCube(*prefilterMap, 1, ShaderStorageAccess::WriteOnly, i);
            prefilterMapShader.dispatch(numGroups, numGroups, 6);
            prefilterMapShader.waitForMemoryBarrier();
        }

        irradianceMap = new TextureCube(TextureFormat::Float32, 32, 32, MipMapFiltering::Bilinear);

        auto& irradianceMapShader = *resourceManager.getResource<ComputeShader>("irradianceMap");
        irradianceMapShader.bind();
        irradianceMapShader.setTextureCube(*prefilterMap, 0);
        irradianceMapShader.setImageCube(*irradianceMap, 1, ShaderStorageAccess::WriteOnly);
        irradianceMapShader.dispatch(irradianceMap->getWidth() / 2, irradianceMap->getWidth() / 2, 6);
        irradianceMapShader.waitForMemoryBarrier();

        resourceManager.insertResource(hdriPath + "-irradiance", irradianceMap);
        resourceManager.insertResource(hdriPath + "-prefilter", prefilterMap);

        return {irradianceMap, prefilterMap};
    }
}
