#pragma once

#include <Rendering/VertexArrayObject.h>

namespace CgEngine {

    struct Vertex {
        float position[3];
        float normal[3];
        float uv[2];

        Vertex(float pX, float pY, float pZ, float nX, float nY, float nZ, float tU, float tV)
        : position{pX, pY, pZ}, normal{nX, nY, nZ}, uv{tU, tV} {};
    };

    class MeshVertices {
    public:
        static MeshVertices* createResource(const std::string& name);

        MeshVertices() = default;
        ~MeshVertices();

        VertexArrayObject* getVAO();
        const std::vector<Vertex>& getVertices() const;
        const std::vector<uint32_t>& getIndexBuffer() const;


    private:
        static MeshVertices* createCubeMesh();

        VertexArrayObject* vao;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indexBuffer;
    };

}
