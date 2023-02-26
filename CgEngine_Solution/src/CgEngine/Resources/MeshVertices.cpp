#include "MeshVertices.h"
#include "Rendering/VertexBuffer.h"

namespace CgEngine {
    MeshVertices* MeshVertices::createResource(const std::string& name) {
        if (name == "CG_CubeMesh") {
            return createCubeMesh();
        }
        return nullptr;
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

    MeshVertices *MeshVertices::createCubeMesh() {
        auto* mesh = new MeshVertices();

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
        mesh->indexBuffer.push_back(0); mesh->indexBuffer.push_back(1); mesh->indexBuffer.push_back(2);
        mesh->indexBuffer.push_back(2); mesh->indexBuffer.push_back(1); mesh->indexBuffer.push_back(3);

        // right
        mesh->indexBuffer.push_back(5); mesh->indexBuffer.push_back(7); mesh->indexBuffer.push_back(6);
        mesh->indexBuffer.push_back(4); mesh->indexBuffer.push_back(5); mesh->indexBuffer.push_back(6);

        // left
        mesh->indexBuffer.push_back(9); mesh->indexBuffer.push_back(8); mesh->indexBuffer.push_back(11);
        mesh->indexBuffer.push_back(8); mesh->indexBuffer.push_back(10); mesh->indexBuffer.push_back(11);

        // back
        mesh->indexBuffer.push_back(13); mesh->indexBuffer.push_back(15); mesh->indexBuffer.push_back(12);
        mesh->indexBuffer.push_back(14); mesh->indexBuffer.push_back(12); mesh->indexBuffer.push_back(15);

        // top
        mesh->indexBuffer.push_back(17); mesh->indexBuffer.push_back(16); mesh->indexBuffer.push_back(18);
        mesh->indexBuffer.push_back(17); mesh->indexBuffer.push_back(18); mesh->indexBuffer.push_back(19);

        // bottom
        mesh->indexBuffer.push_back(21); mesh->indexBuffer.push_back(22); mesh->indexBuffer.push_back(20);
        mesh->indexBuffer.push_back(22); mesh->indexBuffer.push_back(21); mesh->indexBuffer.push_back(23);

        mesh->vao = new VertexArrayObject();

        auto vertexBuffer = std::make_shared<VertexBuffer>(mesh->vertices.data(), mesh->vertices.size() * sizeof(Vertex));
        vertexBuffer->setLayout({VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float3, true),
                                 VertexBufferElement(ShaderDataType::Float2, true)});

        mesh->vao->addVertexBuffer(vertexBuffer);
        mesh->vao->setIndexBuffer(mesh->indexBuffer.data(), mesh->indexBuffer.size());

        return mesh;
    }
}
