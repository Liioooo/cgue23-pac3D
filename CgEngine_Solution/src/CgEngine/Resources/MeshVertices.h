#pragma once

#include <Rendering/VertexArrayObject.h>
#include <Rendering/Material.h>

namespace CgEngine {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 bitangent;

        Vertex(float pX, float pY, float pZ, float nX, float nY, float nZ, float tU, float tV)
        : position{pX, pY, pZ}, normal{nX, nY, nZ}, uv{tU, tV} {};
    };

    struct Submesh {
        uint32_t baseVertex;
        uint32_t baseIndex;
        uint32_t indexCount;
        uint32_t vertexCount;
        uint32_t materialIndex;
        glm::mat4 transform;
    };

    class MeshVertices {
    public:
        static MeshVertices* createResource(const std::string& name);

        MeshVertices() = default;
        ~MeshVertices();

        VertexArrayObject* getVAO();
        const std::vector<Vertex>& getVertices() const;
        const std::vector<uint32_t>& getIndexBuffer() const;
        const std::vector<Submesh>& getSubmeshes() const;
        const std::vector<Material*>& getMaterials() const;


    private:
        static MeshVertices* createCubeMesh();

        VertexArrayObject* vao;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indexBuffer;
        std::vector<Submesh> submeshes;
        std::vector<Material*> materials;
    };

}
