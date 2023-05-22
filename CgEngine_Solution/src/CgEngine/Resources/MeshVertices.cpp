#include "MeshVertices.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/Renderer.h"
#include "Asserts.h"
#include "FileSystem.h"
#include "GlobalObjectManager.h"
#include "Application.h"
#include <filesystem>

namespace CgEngine {
    MeshVertices* MeshVertices::createResource(const std::string &name) {
        if (name == "CG_CubeMesh") {
            return createCubeMesh();
        } else if (name.rfind("CG_SphereMesh", 0) == 0) {
            uint32_t latSegments = 16;
            uint32_t lonSegments = 16;

            if (name.length() > 13) {
                std::string params = name.substr(14);
                if (!params.empty()) {
                    size_t splitPoint = params.find('_');
                    lonSegments = std::stoi(params.substr(0, splitPoint));
                    latSegments = std::stoi(params.substr(splitPoint + 1));
                }
            }

            return createSphereMesh(latSegments, lonSegments);
        } else if (name.rfind("CG_CapsuleMesh", 0) == 0) {
            float radius = 1.0f;
            float height = 1.0f;

            if (name.length() > 14) {
                std::string params = name.substr(15);
                if (!params.empty()) {
                    size_t splitPoint = params.find('_');
                    radius = std::stof(params.substr(0, splitPoint));
                    height = std::stof(params.substr(splitPoint + 1));
                }
            }

            return createCapsuleMesh(height, radius);
        } else {
            return loadMeshAsset(name);
        }
    }

    MeshVertices* MeshVertices::createFromPhysx(const glm::vec3* vertices, uint32_t numVertices, const uint32_t* indices, uint32_t numIndices) {
        auto *mesh = new MeshVertices();

        for (uint32_t i = 0; i < numVertices; i++) {
            mesh->vertices.emplace_back(vertices[i].x, vertices[i].y, vertices[i].z);
        }

        mesh->vao = new VertexArrayObject();

        auto vertexBuffer = std::make_shared<VertexBuffer>(mesh->vertices.data(),
                                                           mesh->vertices.size() * sizeof(Vertex));
        vertexBuffer->setLayout({VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float2, true),
                                 VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float3, true)});

        mesh->vao->addVertexBuffer(vertexBuffer);
        mesh->vao->setIndexBuffer(indices, numIndices);

        Submesh& submesh = mesh->submeshes.emplace_back();
        submesh.baseVertex = 0;
        submesh.baseIndex = 0;
        submesh.vertexCount = numVertices;
        submesh.indexCount = numIndices;
        submesh.materialIndex = 0;

        mesh->materials.emplace_back(Renderer::getDefaultPBRMaterial());

        MeshNode& meshNode = mesh->meshNodes.emplace_back();
        meshNode.aiNode = nullptr;
        meshNode.submeshIndices.push_back(0);
        meshNode.transform = glm::mat4(1.0f);
        meshNode.localTransform = glm::mat4(1.0f);

        return mesh;
    }

    MeshVertices::~MeshVertices() {
        delete vao;
    }

    VertexArrayObject* MeshVertices::getVAO() {
        return vao;
    }

    const std::vector<Vertex>& MeshVertices::getVertices() const {
        return vertices;
    }

    const std::vector<uint32_t>& MeshVertices::getIndexBuffer() const {
        return indexBuffer;
    }

    const std::vector<Submesh>& MeshVertices::getSubmeshes() const {
        return submeshes;
    }

    std::vector<MeshNode>& MeshVertices::getMeshNodes() {
        return meshNodes;
    }

    uint32_t MeshVertices::getMeshNodeIndex(const std::string& nodeName) const {
        return nodeNameToNode.at(nodeName);
    }

    const Material* MeshVertices::getMaterial(size_t index) const {
        return materials.at(index).get();
    }

    PhysicsTriangleMesh& MeshVertices::getPhysicsTriangleMeshForNode(const std::string& nodeName) {
        uint32_t nodeIndex = nodeNameToNode.at(nodeName);
        auto& meshNode = meshNodes.at(nodeIndex);

        CG_ASSERT(meshNode.submeshIndices.size() > 0, "Cannot create PhysicsTriangleMesh from empty node")

        if (meshNode.physicsTriangleMesh != nullptr) {
            return *meshNode.physicsTriangleMesh;
        }

        uint32_t vertexOffset = 0;
        std::vector<glm::vec3> physicsVertices;
        std::vector<uint32_t> physicsIndices;

        for (const auto& smi: meshNode.submeshIndices) {
            auto& submesh = submeshes.at(smi);

            for (uint32_t i = submesh.baseVertex; i < submesh.baseVertex + submesh.vertexCount; i++) {
                physicsVertices.emplace_back(meshNode.transform * glm::vec4(vertices.at(i).position, 1.0f));
            }

            for (uint32_t i = submesh.baseIndex; i < submesh.baseIndex + submesh.indexCount; i++) {
                physicsIndices.emplace_back(indexBuffer.at(i) + vertexOffset);
            }

            vertexOffset += submesh.vertexCount;
        }

        auto& physicsCooking = GlobalObjectManager::getInstance().getPhysicsSystem().getPhysicsCooking();
        auto* physicsMesh = physicsCooking.cookTriangleMesh(physicsVertices.data(), physicsVertices.size(), physicsIndices.data(), physicsIndices.size());
        meshNode.physicsTriangleMesh = physicsMesh;
        return *physicsMesh;
    }

    PhysicsConvexMesh& MeshVertices::getPhysicsConvexMeshForNode(const std::string& nodeName) {
        uint32_t nodeIndex = nodeNameToNode.at(nodeName);
        auto& meshNode = meshNodes.at(nodeIndex);

        CG_ASSERT(meshNode.submeshIndices.size() > 0, "Cannot create PhysicsConvexMesh from empty node")

        if (meshNode.physicsConvexMesh != nullptr) {
            return *meshNode.physicsConvexMesh;
        }

        std::vector<glm::vec3> physicsVertices;

        for (const auto& smi: meshNode.submeshIndices) {
            auto& submesh = submeshes.at(smi);

            for (uint32_t i = submesh.baseVertex; i < submesh.baseVertex + submesh.vertexCount; i++) {
                physicsVertices.emplace_back(meshNode.transform * glm::vec4(vertices.at(i).position, 1.0f));
            }
        }

        auto& physicsCooking = GlobalObjectManager::getInstance().getPhysicsSystem().getPhysicsCooking();
        auto* physicsMesh = physicsCooking.cookConvexMesh(physicsVertices.data(), physicsVertices.size());
        meshNode.physicsConvexMesh = physicsMesh;
        return *physicsMesh;
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

        Submesh& submesh = mesh->submeshes.emplace_back();
        submesh.baseVertex = 0;
        submesh.baseIndex = 0;
        submesh.vertexCount = mesh->vertices.size();
        submesh.indexCount = mesh->indexBuffer.size();
        submesh.materialIndex = 0;

        mesh->materials.emplace_back(Renderer::getDefaultPBRMaterial());

        MeshNode& meshNode = mesh->meshNodes.emplace_back();
        meshNode.aiNode = nullptr;
        meshNode.submeshIndices.push_back(0);
        meshNode.transform = glm::mat4(1.0f);
        meshNode.localTransform = glm::mat4(1.0f);

        return mesh;
    }

    MeshVertices* MeshVertices::createSphereMesh(uint32_t latSegments, uint32_t lonSegments) {
        auto *mesh = new MeshVertices();

        mesh->vertices.emplace_back(0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
        mesh->vertices.emplace_back(0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);

        // latitude
        for (uint32_t i = 1; i < latSegments; i++) {
            float latAngle = glm::pi<float>() * i / latSegments;
            float xz = glm::sin(latAngle);
            float y = glm::cos(latAngle);

            // longitude
            for (uint32_t j = 0; j < lonSegments; j++) {
                float lonAngle = glm::two_pi<float>() * j / lonSegments;
                glm::vec3 vertexPos(xz * glm::sin(lonAngle), y, xz * glm::cos(lonAngle));
                mesh->vertices.emplace_back(
                        vertexPos.x, vertexPos.y, vertexPos.z,
                        vertexPos.x, vertexPos.y, vertexPos.z,
                        glm::atan(vertexPos.x, vertexPos.z) / glm::two_pi<float>() + 0.5f, 0.5f + glm::asin(vertexPos.y) / glm::pi<float>()
                );
            }
        }

        for (uint32_t i = 2; i < 2 + lonSegments; i++) {
            mesh->indexBuffer.push_back(0);
            mesh->indexBuffer.push_back(i);
            if (i == 1 + lonSegments) {
                mesh->indexBuffer.push_back(2);
            } else {
                mesh->indexBuffer.push_back(i + 1);
            }
        }

        for (uint32_t i = 2 + (latSegments - 2) * lonSegments; i < 2 + (latSegments - 1) * lonSegments; i++) {
            mesh->indexBuffer.push_back(1);
            if (i == 1 + (latSegments - 1) * lonSegments) {
                mesh->indexBuffer.push_back(2 + (latSegments - 2) * lonSegments);
            } else {
                mesh->indexBuffer.push_back(i + 1);
            }
            mesh->indexBuffer.push_back(i);
        }

        for (uint32_t i = 0; i < latSegments - 2; i++) {
            for (int j = 2 + i * lonSegments; j < 2 + i * lonSegments + lonSegments; j++) {
                mesh->indexBuffer.push_back(j);
                mesh->indexBuffer.push_back(j + lonSegments);
                if (j == 1 + i * lonSegments + lonSegments) {
                    mesh->indexBuffer.push_back(2 + i * lonSegments);
                } else {
                    mesh->indexBuffer.push_back(j + 1);
                }

                if (j == 1 + i * lonSegments + lonSegments) {
                    mesh->indexBuffer.push_back(j + lonSegments);
                    mesh->indexBuffer.push_back(2 + i * lonSegments + lonSegments);
                    mesh->indexBuffer.push_back(2 + i * lonSegments);
                } else {
                    mesh->indexBuffer.push_back(j + lonSegments);
                    mesh->indexBuffer.push_back(j + lonSegments + 1);
                    mesh->indexBuffer.push_back(j + 1);
                }
            }
        }

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

        MeshNode& meshNode = mesh->meshNodes.emplace_back();
        meshNode.aiNode = nullptr;
        meshNode.submeshIndices.push_back(0);
        meshNode.transform = glm::mat4(1.0f);
        meshNode.localTransform = glm::mat4(1.0f);

        return mesh;
    }

    MeshVertices* MeshVertices::createCapsuleMesh(float height, float radius) {
        auto *mesh = new MeshVertices();

        const uint32_t latSegments = 8;
        const uint32_t lonSegments = 16;

        mesh->vertices.emplace_back(0.0f, height / 2 + radius, 0.0f, 0.0f, 1.0f, 0.0f);

        // latitude
        for (uint32_t i = 1; i <= latSegments; i++) {
            float latAngle = i * (glm::half_pi<float>() / latSegments);
            float xz = glm::sin(latAngle);
            float y = glm::cos(latAngle);

            // longitude
            for (uint32_t j = 0; j < lonSegments; j++) {
                float lonAngle = glm::two_pi<float>() * j / lonSegments;
                glm::vec3 vertexPos(xz * glm::sin(lonAngle), y, xz * glm::cos(lonAngle));
                mesh->vertices.emplace_back(
                        radius * vertexPos.x, radius * vertexPos.y + height / 2, radius * vertexPos.z,
                        vertexPos.x, vertexPos.y, vertexPos.z
                );
            }
        }

        mesh->vertices.emplace_back(0.0f, -(height / 2 + radius), 0.0f, 0.0f, -1.0f, 0.0f);

        // latitude
        for (uint32_t i = 1; i <= latSegments; i++) {
            float latAngle = glm::pi<float>() - i * (glm::half_pi<float>() / latSegments);
            float xz = glm::sin(latAngle);
            float y = glm::cos(latAngle);

            // longitude
            for (uint32_t j = 0; j < lonSegments; j++) {
                float lonAngle = glm::two_pi<float>() * j / lonSegments;
                glm::vec3 vertexPos(xz * glm::sin(lonAngle), y, xz * glm::cos(lonAngle));
                mesh->vertices.emplace_back(
                        radius * vertexPos.x, radius * vertexPos.y - height / 2, radius * vertexPos.z,
                        vertexPos.x, vertexPos.y, vertexPos.z
                );
            }
        }

        for (uint32_t i = 1; i < 1 + lonSegments; i++) {
            mesh->indexBuffer.push_back(0);
            mesh->indexBuffer.push_back(i);
            if (i == lonSegments) {
                mesh->indexBuffer.push_back(1);
            } else {
                mesh->indexBuffer.push_back(i + 1);
            }
        }

        for (uint32_t i = 0; i < latSegments - 1; i++) {
            for (int j = 1 + i * lonSegments; j < 1 + i * lonSegments + lonSegments; j++) {
                if (j == i * lonSegments + lonSegments) {
                    mesh->indexBuffer.push_back(j);
                    mesh->indexBuffer.push_back(j + lonSegments);
                    mesh->indexBuffer.push_back(j + 1 - lonSegments);
                } else {
                    mesh->indexBuffer.push_back(j);
                    mesh->indexBuffer.push_back(j + lonSegments);
                    mesh->indexBuffer.push_back(j + 1);
                }

                if (j == i * lonSegments + lonSegments) {
                    mesh->indexBuffer.push_back(j + lonSegments);
                    mesh->indexBuffer.push_back(1 + i * lonSegments + lonSegments);
                    mesh->indexBuffer.push_back(1 + i * lonSegments);
                } else {
                    mesh->indexBuffer.push_back(j + lonSegments);
                    mesh->indexBuffer.push_back(j + lonSegments + 1);
                    mesh->indexBuffer.push_back(j + 1);
                }
            }
        }

        for (uint32_t i = latSegments * lonSegments + 2; i < latSegments * lonSegments + 2 + lonSegments; i++) {
            mesh->indexBuffer.push_back(i);
            mesh->indexBuffer.push_back(latSegments * lonSegments + 1);
            if (i == latSegments * lonSegments + 1 + lonSegments) {
                mesh->indexBuffer.push_back(latSegments * lonSegments + 2);
            } else {
                mesh->indexBuffer.push_back(i + 1);
            }
        }

        for (uint32_t i = 0; i < latSegments - 1; i++) {
            for (uint32_t j = latSegments * lonSegments + 2 + lonSegments * i; j < latSegments * lonSegments + 2 + lonSegments * i + lonSegments; j++) {
                if (j == latSegments * lonSegments + 1 + lonSegments * i + lonSegments) {
                    mesh->indexBuffer.push_back(j);
                    mesh->indexBuffer.push_back(1 + j - lonSegments);
                    mesh->indexBuffer.push_back(j + lonSegments);
                } else {
                    mesh->indexBuffer.push_back(j);
                    mesh->indexBuffer.push_back(j + 1);
                    mesh->indexBuffer.push_back(j + lonSegments);
                }

                if (j == latSegments * lonSegments + 1 + lonSegments * i + lonSegments) {
                    mesh->indexBuffer.push_back(j + 1 - lonSegments);
                    mesh->indexBuffer.push_back(j + 1);
                    mesh->indexBuffer.push_back(j + lonSegments);
                } else {
                    mesh->indexBuffer.push_back(j + lonSegments);
                    mesh->indexBuffer.push_back(j + 1);
                    mesh->indexBuffer.push_back(j + lonSegments + 1);
                }
            }
        }

        for (uint32_t i = (latSegments - 1) * lonSegments + 1, j = 0; i < latSegments * lonSegments + 1; i++, j++) {
            if (i == latSegments * lonSegments) {
                mesh->indexBuffer.push_back(i + 1 - lonSegments);
                mesh->indexBuffer.push_back(i);
                mesh->indexBuffer.push_back(mesh->vertices.size() - lonSegments + j);
            } else {
                mesh->indexBuffer.push_back(i + 1);
                mesh->indexBuffer.push_back(i);
                mesh->indexBuffer.push_back(mesh->vertices.size() - lonSegments + j);
            }

            if (i == latSegments * lonSegments) {
                mesh->indexBuffer.push_back(i + 1 - lonSegments);
                mesh->indexBuffer.push_back(mesh->vertices.size() - lonSegments + j);
                mesh->indexBuffer.push_back(mesh->vertices.size() - lonSegments);
            } else {
                mesh->indexBuffer.push_back(mesh->vertices.size() - lonSegments + j);
                mesh->indexBuffer.push_back(mesh->vertices.size() - lonSegments + j + 1);
                mesh->indexBuffer.push_back(i + 1);
            }
        }

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

        MeshNode& meshNode = mesh->meshNodes.emplace_back();
        meshNode.aiNode = nullptr;
        meshNode.submeshIndices.push_back(0);
        meshNode.transform = glm::mat4(1.0f);
        meshNode.localTransform = glm::mat4(1.0f);

        return mesh;
    }

    MeshVertices *MeshVertices::loadMeshAsset(const std::string &path) {
        auto importer = Assimp::Importer();

        std::string modelPath = FileSystem::getAsGamePath(path);

        const aiScene *scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_SortByPType |
                                                            aiProcess_GenNormals | aiProcess_FlipUVs |
                                                            aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices);

        CG_ASSERT(scene && scene->HasMeshes() && scene->mRootNode, "3D-Asset could not be loaded: " + path + ", " + importer.GetErrorString());

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
            auto& applicationOptions = Application::get().getApplicationOptions();

            for (uint32_t m = 0; m < scene->mNumMaterials; m++) {
                aiMaterial *aiMaterial = scene->mMaterials[m];

                auto &material = mesh->materials.emplace_back(new Material(aiMaterial->GetName().C_Str()));

                float aiEmissionIntensity;
                bool hasEmissionIntensity = aiMaterial->Get(AI_MATKEY_EMISSIVE_INTENSITY, aiEmissionIntensity) == AI_SUCCESS;
                aiColor3D aiEmissionColor;
                bool hasEmissionColor = aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmissionColor) == AI_SUCCESS;
                aiString aiEmissiveTexPath;
                aiTextureMapMode aiEmissiveWrapMode[3];
                bool hasEmissiveTexture = aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &aiEmissiveTexPath, nullptr, nullptr, nullptr, nullptr, aiEmissiveWrapMode) == AI_SUCCESS;
                bool useEmissiveColor = !hasEmissiveTexture;
                if (hasEmissiveTexture) {
                    std::string texturePath = getTexturePath(modelPath, aiEmissiveTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture2D("u_Mat_EmissionTexture", *resourceManager.getResource<Texture2D>(texturePath), 4);
                        if (hasEmissionIntensity) {
                            material->set("u_Mat_Emission", {aiEmissionIntensity, aiEmissionIntensity, aiEmissionIntensity});
                        } else {
                            material->set("u_Mat_Emission", {1.0f, 1.0f, 1.0f});
                        }
                    } else {
                        auto *texture = new Texture2D(texturePath, true, getTextureWrapFromAssimp(aiEmissiveWrapMode[0]), MipMapFiltering::Trilinear, applicationOptions.anisotropicFiltering);
                        if (texture->isLoaded()) {
                            material->setTexture2D("u_Mat_EmissionTexture", *texture, 4);
                            if (hasEmissionIntensity) {
                                material->set("u_Mat_Emission", {aiEmissionIntensity, aiEmissionIntensity, aiEmissionIntensity});
                            } else {
                                material->set("u_Mat_Emission", {1.0f, 1.0f, 1.0f});
                            }
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            useEmissiveColor = true;
                            delete texture;
                        }
                    }
                }
                if (useEmissiveColor) {
                    material->setTexture2D("u_Mat_EmissionTexture", Renderer::getWhiteTexture(), 4);
                    if (hasEmissionColor) {
                        if (hasEmissionIntensity) {
                            material->set("u_Mat_Emission", glm::vec3(aiEmissionColor.r, aiEmissionColor.g, aiEmissionColor.b) * aiEmissionIntensity);
                        } else {
                            material->set("u_Mat_Emission", {aiEmissionColor.r, aiEmissionColor.g, aiEmissionColor.b});
                        }
                    } else {
                        material->set("u_Mat_Emission", {0.0f, 0.0f, 0.0f});
                    }
                }

                aiColor3D aiAlbedo;
                if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiAlbedo) != AI_SUCCESS) {
                    aiAlbedo = {1.0f, 1.0f, 1.0f};
                }
                aiString aiAlbedoTexPath;
                aiTextureMapMode aiAlbedoWrapMode[3];
                bool hasAlbedoTexture = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiAlbedoTexPath, nullptr, nullptr, nullptr, nullptr, aiAlbedoWrapMode) == AI_SUCCESS;
                bool useAlbedoColor = !hasAlbedoTexture;
                if (hasAlbedoTexture) {
                    std::string texturePath = getTexturePath(modelPath, aiAlbedoTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture2D("u_Mat_AlbedoTexture", *resourceManager.getResource<Texture2D>(texturePath), 0);
                        material->set("u_Mat_AlbedoColor", {1.0f, 1.0f, 1.0f});
                    } else {
                        auto *texture = new Texture2D(texturePath, true, getTextureWrapFromAssimp(aiAlbedoWrapMode[0]), MipMapFiltering::Trilinear, applicationOptions.anisotropicFiltering);
                        if (texture->isLoaded()) {
                            material->setTexture2D("u_Mat_AlbedoTexture", *texture, 0);
                            material->set("u_Mat_AlbedoColor", {1.0f, 1.0f, 1.0f});
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            useAlbedoColor = true;
                            delete texture;
                        }
                    }
                }
                if (useAlbedoColor) {
                    material->setTexture2D("u_Mat_AlbedoTexture", Renderer::getWhiteTexture(), 0);
                    material->set("u_Mat_AlbedoColor", {aiAlbedo.r, aiAlbedo.g, aiAlbedo.b});
                }

                float shininess, roughness;
                if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS) {
                    roughness = 1.0f;
                } else {
                    roughness = 1.0f - glm::sqrt(shininess / 100.0f);
                }
                aiString aiRoughnessTexPath;
                aiTextureMapMode aiRoughnessWrapMode[3];
                bool hasRoughnessTexture = aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiRoughnessTexPath, nullptr, nullptr, nullptr, nullptr, aiRoughnessWrapMode) == AI_SUCCESS;
                bool useRoughnessValue = !hasRoughnessTexture;
                if (hasRoughnessTexture) {
                    std::string texturePath = getTexturePath(modelPath, aiRoughnessTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture2D("u_Mat_RoughnessTexture", *resourceManager.getResource<Texture2D>(texturePath), 3);
                        material->set("u_Mat_Roughness", 1.0f);
                    } else {
                        auto *texture = new Texture2D(texturePath, false, getTextureWrapFromAssimp(aiRoughnessWrapMode[0]), MipMapFiltering::Trilinear, applicationOptions.anisotropicFiltering);
                        if (texture->isLoaded()) {
                            material->setTexture2D("u_Mat_RoughnessTexture", *texture, 3);
                            material->set("u_Mat_Roughness", 1.0f);
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            useRoughnessValue = true;
                            delete texture;
                        }
                    }
                }
                if (useRoughnessValue) {
                    material->setTexture2D("u_Mat_RoughnessTexture", Renderer::getWhiteTexture(), 3);
                    material->set("u_Mat_Roughness", roughness);
                }

                aiString aiNormalTexPath;
                aiTextureMapMode aiNormalWrapMode[3];
                bool hasNormalMap = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiNormalTexPath, nullptr, nullptr, nullptr, nullptr, aiNormalWrapMode) == AI_SUCCESS;
                bool dontUseNormals = !hasNormalMap;
                if (hasNormalMap) {
                    std::string texturePath = getTexturePath(modelPath, aiNormalTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture2D("u_Mat_NormalTexture", *resourceManager.getResource<Texture2D>(texturePath), 1);
                        material->set("u_Mat_UseNormals", true);
                    } else {
                        auto *texture = new Texture2D(texturePath, false, getTextureWrapFromAssimp(aiNormalWrapMode[0]), MipMapFiltering::Trilinear, applicationOptions.anisotropicFiltering);
                        if (texture->isLoaded()) {
                            material->setTexture2D("u_Mat_NormalTexture", *texture, 1);
                            material->set("u_Mat_UseNormals", true);
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            dontUseNormals = true;
                            delete texture;
                        }
                    }
                }
                if (dontUseNormals) {
                    material->setTexture2D("u_Mat_NormalTexture", Renderer::getWhiteTexture(), 1);
                    material->set("u_Mat_UseNormals", false);
                }


                float metalness;
                if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != AI_SUCCESS) {
                    metalness = 0.0f;
                }
                aiString aiMetalnessTexPath;
                aiTextureMapMode aiMetalnessWrapMode[3];
                bool hasMetalnessTexture = aiMaterial->GetTexture(aiTextureType_METALNESS, 0, &aiMetalnessTexPath, nullptr, nullptr, nullptr, nullptr, aiMetalnessWrapMode) == AI_SUCCESS;
                bool useMetalnessValue = !hasMetalnessTexture;
                if (hasMetalnessTexture) {
                    std::string texturePath = getTexturePath(modelPath, aiMetalnessTexPath.C_Str());
                    if (resourceManager.hasResource<Texture2D>(texturePath)) {
                        material->setTexture2D("u_Mat_MetalnessTexture", *resourceManager.getResource<Texture2D>(texturePath), 2);
                        material->set("u_Mat_Metalness", 1.0f);
                    } else {
                        auto *texture = new Texture2D(texturePath, false, getTextureWrapFromAssimp(aiMetalnessWrapMode[0]), MipMapFiltering::Trilinear, applicationOptions.anisotropicFiltering);
                        if (texture->isLoaded()) {
                            material->setTexture2D("u_Mat_MetalnessTexture", *texture, 2);
                            material->set("u_Mat_Metalness", 1.0f);
                            resourceManager.insertResource<Texture2D>(texturePath, texture);
                        } else {
                            useMetalnessValue = true;
                            delete texture;
                        }
                    }
                }
                if (useMetalnessValue) {
                    material->setTexture2D("u_Mat_MetalnessTexture", Renderer::getWhiteTexture(), 2);
                    material->set("u_Mat_Metalness", metalness);
                }
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

    TextureWrap MeshVertices::getTextureWrapFromAssimp(aiTextureMapMode mapMode) {
        switch (mapMode) {
            case aiTextureMapMode_Clamp:
                return TextureWrap::Clamp;
            case aiTextureMapMode_Wrap:
                return TextureWrap::Repeat;
        }

        return TextureWrap::Repeat;
    }

    void MeshVertices::traverseNodes(aiNode *node, const glm::mat4 &parentTransform) {
        glm::mat4 localTransform = getTransformFromAssimpTransform(node->mTransformation);
        glm::mat4 transform = parentTransform * localTransform;

        MeshNode& meshNode = meshNodes.emplace_back();
        meshNode.aiNode = node;
        meshNode.localTransform = localTransform;
        meshNode.transform = transform;

        nodeNameToNode.insert({node->mName.C_Str(), meshNodes.size() - 1});

        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            uint32_t submeshIndex = node->mMeshes[i];
            meshNode.submeshIndices.push_back(submeshIndex);
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            traverseNodes(node->mChildren[i], transform);
        }
    }
}
