#include "MeshVertices.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/Renderer.h"
#include "Asserts.h"
#include <filesystem>
#include "FileSystem.h"
#include <GlobalObjectManager.h>

namespace CgEngine {
    MeshVertices *MeshVertices::createResource(const std::string &name) {
        if (name == "CG_CubeMesh") {
            return createCubeMesh();
        } else {
            return loadMeshAsset(name);
        }
    }

    MeshVertices::~MeshVertices() {
        delete vao;
    }

    VertexArrayObject *MeshVertices::getVAO() {
        return vao;
    }

    const std::vector<Vertex> &MeshVertices::getVertices() const {
        return vertices;
    }

    const std::vector<uint32_t> &MeshVertices::getIndexBuffer() const {
        return indexBuffer;
    }

    const std::vector<Submesh> &MeshVertices::getSubmeshes() const {
        return submeshes;
    }

    const Material *MeshVertices::getMaterial(size_t index) const {
        return materials.at(index).get();
    }

    MeshVertices *MeshVertices::createCubeMesh() {
        auto *mesh = new MeshVertices();

        // Front Face
        mesh->vertices.emplace_back(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f); // left_top_front
        mesh->vertices.emplace_back(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f); // left_bottom_front
        mesh->vertices.emplace_back(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f); // right_top_front
        mesh->vertices.emplace_back(0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f); // right_bottom_front

        // Right Face
        mesh->vertices.emplace_back(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f); // right_top_front
        mesh->vertices.emplace_back(0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f); // right_bottom_front
        mesh->vertices.emplace_back(0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f); // right_top_back
        mesh->vertices.emplace_back(0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f); // right_bottom_back

        // Left Face
        mesh->vertices.emplace_back(-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f); // left_top_front
        mesh->vertices.emplace_back(-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f); // left_bottom_front
        mesh->vertices.emplace_back(-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f); // left_top_back
        mesh->vertices.emplace_back(-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f); // left_bottom_back

        // Back Face
        mesh->vertices.emplace_back(0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f); // right_top_back
        mesh->vertices.emplace_back(0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f); // right_bottom_back
        mesh->vertices.emplace_back(-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f); // left_top_back
        mesh->vertices.emplace_back(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f); // left_bottom_back

        // Top Face
        mesh->vertices.emplace_back(0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f); // right_top_front
        mesh->vertices.emplace_back(-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f); // left_top_front
        mesh->vertices.emplace_back(0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f); // right_top_back
        mesh->vertices.emplace_back(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f); // left_top_back

        // Bottom Face
        mesh->vertices.emplace_back(0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f); // right_bottom_front
        mesh->vertices.emplace_back(-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f); // left_bottom_front
        mesh->vertices.emplace_back(0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f); // right_bottom_back
        mesh->vertices.emplace_back(-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f); // left_bottom_back

        // front
        mesh->indexBuffer.push_back(0);
        mesh->indexBuffer.push_back(1);
        mesh->indexBuffer.push_back(2);
        mesh->indexBuffer.push_back(2);
        mesh->indexBuffer.push_back(1);
        mesh->indexBuffer.push_back(3);

        // right
        mesh->indexBuffer.push_back(5);
        mesh->indexBuffer.push_back(7);
        mesh->indexBuffer.push_back(6);
        mesh->indexBuffer.push_back(4);
        mesh->indexBuffer.push_back(5);
        mesh->indexBuffer.push_back(6);

        // left
        mesh->indexBuffer.push_back(9);
        mesh->indexBuffer.push_back(8);
        mesh->indexBuffer.push_back(11);
        mesh->indexBuffer.push_back(8);
        mesh->indexBuffer.push_back(10);
        mesh->indexBuffer.push_back(11);

        // back
        mesh->indexBuffer.push_back(13);
        mesh->indexBuffer.push_back(15);
        mesh->indexBuffer.push_back(12);
        mesh->indexBuffer.push_back(14);
        mesh->indexBuffer.push_back(12);
        mesh->indexBuffer.push_back(15);

        // top
        mesh->indexBuffer.push_back(17);
        mesh->indexBuffer.push_back(16);
        mesh->indexBuffer.push_back(18);
        mesh->indexBuffer.push_back(17);
        mesh->indexBuffer.push_back(18);
        mesh->indexBuffer.push_back(19);

        // bottom
        mesh->indexBuffer.push_back(21);
        mesh->indexBuffer.push_back(22);
        mesh->indexBuffer.push_back(20);
        mesh->indexBuffer.push_back(22);
        mesh->indexBuffer.push_back(21);
        mesh->indexBuffer.push_back(23);

        mesh->vao = new VertexArrayObject();

        auto vertexBuffer = std::make_shared<VertexBuffer>(mesh->vertices.data(),
                                                           mesh->vertices.size() * sizeof(Vertex));
        vertexBuffer->setLayout({VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float2, true),
                                 VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float3, true)});

        mesh->vao->addVertexBuffer(vertexBuffer);
        mesh->vao->setIndexBuffer(mesh->indexBuffer.data(), mesh->indexBuffer.size());

        Submesh &submesh = mesh->submeshes.emplace_back();
        submesh.baseVertex = 0;
        submesh.baseIndex = 0;
        submesh.vertexCount = mesh->vertices.size();
        submesh.indexCount = mesh->indexBuffer.size();
        submesh.materialIndex = 0;

        mesh->materials.emplace_back(Renderer::getDefaultPBRMaterial());

        return mesh;
    }

    MeshVertices *MeshVertices::loadMeshAsset(const std::string &path) {
        auto importer = Assimp::Importer();

        std::string modelPath = FileSystem::getAsGamePath(path);

        const aiScene *scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_SortByPType |
                                                            aiProcess_GenNormals  | aiProcess_FlipUVs |
                                                            aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices);

        CG_ASSERT(scene && scene->HasMeshes() && scene->mRootNode,
                  "3D-Asset could not be loaded: " + path + ", " + importer.GetErrorString());

        auto* mesh = new MeshVertices();

        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *aiMesh = scene->mMeshes[i];

            Submesh& submesh = mesh->submeshes.emplace_back();
            submesh.baseIndex = indexCount;
            submesh.baseVertex = vertexCount;
            submesh.vertexCount = aiMesh->mNumVertices;
            submesh.indexCount = aiMesh->mNumFaces * 3;
            submesh.materialIndex = aiMesh->mMaterialIndex;

            vertexCount += aiMesh->mNumVertices;
            indexCount += submesh.indexCount;

            for (size_t v = 0; v < aiMesh->mNumVertices; v++) {
                Vertex& vertex = mesh->vertices.emplace_back();
                vertex.position = {aiMesh->mVertices[v].x, aiMesh->mVertices[v].y, aiMesh->mVertices[v].z};
                vertex.normal = {aiMesh->mNormals[v].x, aiMesh->mNormals[v].y, aiMesh->mNormals[v].z};

                if (aiMesh->HasTextureCoords(0)) {
                    vertex.uv = {aiMesh->mTextureCoords[0][v].x, aiMesh->mTextureCoords[0][v].y};
                }

                if (aiMesh->HasTangentsAndBitangents()) {
                    vertex.tangent = {aiMesh->mTangents[v].x, aiMesh->mTangents[v].y, aiMesh->mTangents[v].z};
                    vertex.bitangent = {aiMesh->mBitangents[v].x, aiMesh->mBitangents[v].y, aiMesh->mBitangents[v].z};
                }
            }

            for (size_t f = 0; f < aiMesh->mNumFaces; f++) {
                mesh->indexBuffer.push_back(aiMesh->mFaces[f].mIndices[0]);
                mesh->indexBuffer.push_back(aiMesh->mFaces[f].mIndices[1]);
                mesh->indexBuffer.push_back(aiMesh->mFaces[f].mIndices[2]);
            }
        }

        mesh->traverseNodes(scene->mRootNode, glm::mat4(1.0f));

        mesh->vao = new VertexArrayObject();

        auto vertexBuffer = std::make_shared<VertexBuffer>(mesh->vertices.data(), mesh->vertices.size() * sizeof(Vertex));
        vertexBuffer->setLayout({VertexBufferElement(ShaderDataType::Float3, false),
                                 VertexBufferElement(ShaderDataType::Float3, false),
                                 VertexBufferElement(ShaderDataType::Float2, false),
                                 VertexBufferElement(ShaderDataType::Float3, false),
                                 VertexBufferElement(ShaderDataType::Float3, false)});

        mesh->vao->addVertexBuffer(vertexBuffer);
        mesh->vao->setIndexBuffer(mesh->indexBuffer.data(), mesh->indexBuffer.size());

        auto resourceManager = GlobalObjectManager::getInstance().getResourceManager();

        if (scene->HasMaterials()) {
            for (uint32_t m = 0; m < scene->mNumMaterials; m++) {
                aiMaterial *aiMaterial = scene->mMaterials[m];

                auto &material = mesh->materials.emplace_back(new Material(aiMaterial->GetName().C_Str()));

                aiColor3D aiEmission;
                if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmission) == AI_SUCCESS) {
                    material->set("u_Mat_Emission", aiEmission.r);
                } else {
                    material->set("u_Mat_Emission", 0.0f);
                }

                aiColor3D aiAlbedo;
                if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiAlbedo) != AI_SUCCESS) {
                    aiAlbedo = {1.0f, 1.0f, 1.0f};
                }
                aiString aiAlbedoTexPath;
                bool hasAlbedoTexture =
                        aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiAlbedoTexPath) == AI_SUCCESS;
                bool useAlbedoColor = !hasAlbedoTexture;
                if (hasAlbedoTexture) {
                    std::string texturePath = getTexturePath(modelPath, aiAlbedoTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture("u_Mat_AlbedoTexture", *resourceManager.getResource<Texture2D>(path), 0);
                        material->set("u_Mat_AlbedoColor", {1.0f, 1.0f, 1.0f});
                    } else {
                        auto *texture = new Texture2D(texturePath, true);
                        if (texture->isLoaded()) {
                            material->setTexture("u_Mat_AlbedoTexture", *texture, 0);
                            material->set("u_Mat_AlbedoColor", {1.0f, 1.0f, 1.0f});
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            useAlbedoColor = true;
                            delete texture;
                        }
                    }
                }
                if (useAlbedoColor) {
                    material->setTexture("u_Mat_AlbedoTexture", Renderer::getWhiteTexture(), 0);
                    material->set("u_Mat_AlbedoColor", {aiAlbedo.r, aiAlbedo.g, aiAlbedo.b});
                }

                float shininess, roughness;
                if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS) {
                    roughness = 1.0f;
                } else {
                    roughness = 1.0f - glm::sqrt(shininess / 100.0f);
                }
                aiString aiRoughnessTexPath;
                bool hasRoughnessTexture =
                        aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiRoughnessTexPath) == AI_SUCCESS;
                bool useRoughnessValue = !hasRoughnessTexture;
                if (hasRoughnessTexture) {
                    std::string texturePath = getTexturePath(modelPath, aiRoughnessTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture("u_Mat_RoughnessTexture", *resourceManager.getResource<Texture2D>(path),
                                             3);
                        material->set("u_Mat_Roughness", 1.0f);
                    } else {
                        auto *texture = new Texture2D(texturePath, false);
                        if (texture->isLoaded()) {
                            material->setTexture("u_Mat_RoughnessTexture", *texture, 3);
                            material->set("u_Mat_Roughness", 1.0f);
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            useRoughnessValue = true;
                            delete texture;
                        }
                    }
                }
                if (useRoughnessValue) {
                    material->setTexture("u_Mat_RoughnessTexture", Renderer::getWhiteTexture(), 3);
                    material->set("u_Mat_Roughness", roughness);
                }

                aiString aiNormalTexPath;
                bool hasNormalMap = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiNormalTexPath) == AI_SUCCESS;
                bool dontUseNormals = !hasNormalMap;
                if (hasNormalMap) {
                    std::string texturePath = getTexturePath(modelPath, aiNormalTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture("u_Mat_NormalTexture", *resourceManager.getResource<Texture2D>(path), 1);
                        material->set("u_Mat_UseNormals", true);
                    } else {
                        auto *texture = new Texture2D(texturePath, false);
                        if (texture->isLoaded()) {
                            material->setTexture("u_Mat_NormalTexture", *texture, 1);
                            material->set("u_Mat_UseNormals", true);
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            dontUseNormals = true;
                            delete texture;
                        }
                    }
                }
                if (dontUseNormals) {
                    material->setTexture("u_Mat_NormalTexture", Renderer::getWhiteTexture(), 1);
                    material->set("u_Mat_UseNormals", false);
                }


                float metalness;
                if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != AI_SUCCESS) {
                    metalness = 0.0f;
                }
                material->set("u_Mat_Metalness", metalness);
                material->setTexture("u_Mat_MetalnessTexture", Renderer::getWhiteTexture(), 2);
            }
        } else {
            mesh->materials.emplace_back(Renderer::getDefaultPBRMaterial());
        }

        return mesh;
    }

    std::string MeshVertices::getTexturePath(const std::string &modelPath, const std::string &texturePath) {
        std::filesystem::path path = modelPath;
        auto directory = path.parent_path();
        directory /= texturePath;
        return directory.string();
    }

    glm::mat4 MeshVertices::getTransformFromAssimpTransform(const aiMatrix4x4 &transform) {
        glm::mat4 result;
        result[0][0] = transform.a1; result[1][0] = transform.a2; result[2][0] = transform.a3; result[3][0] = transform.a4;
        result[0][1] = transform.b1; result[1][1] = transform.b2; result[2][1] = transform.b3; result[3][1] = transform.b4;
        result[0][2] = transform.c1; result[1][2] = transform.c2; result[2][2] = transform.c3; result[3][2] = transform.c4;
        result[0][3] = transform.d1; result[1][3] = transform.d2; result[2][3] = transform.d3; result[3][3] = transform.d4;
        return result;
    }

    void MeshVertices::traverseNodes(aiNode *node, const glm::mat4 &parentTransform) {
        glm::mat4 localTransform = getTransformFromAssimpTransform(node->mTransformation);
        glm::mat4 transform = parentTransform * localTransform;

        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            uint32_t submeshIndex = node->mMeshes[i];
            Submesh& submesh = submeshes.at(submeshIndex);
            submesh.localTransform = localTransform;
            submesh.transform = transform;
            nodeToSubmesh[node].push_back(submeshIndex);
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            traverseNodes(node->mChildren[i], transform);
        }
    }
}
