#pragma once

#include "Rendering/VertexArrayObject.h"
#include "Rendering/Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace CgEngine {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 tangent;
        glm::vec3 bitangent;

        Vertex() {};
        Vertex(float pX, float pY, float pZ, float nX, float nY, float nZ, float tU, float tV)
        : position{pX, pY, pZ}, normal{nX, nY, nZ}, uv{tU, tV} {};

        Vertex(float pX, float pY, float pZ, float nX, float nY, float nZ)
                : position{pX, pY, pZ}, normal{nX, nY, nZ} {};
    };

    struct Submesh {
        uint32_t baseVertex;
        uint32_t baseIndex;
        uint32_t indexCount;
        uint32_t vertexCount;
        uint32_t materialIndex;
    };

    struct MeshNode {
        aiNode* aiNode;
        std::vector<uint32_t> submeshIndices;
        glm::mat4 localTransform{1.0f};
        glm::mat4 transform{1.0f};
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
        std::vector<MeshNode>& getMeshNodes();
        const Material* getMaterial(size_t index) const;


    private:
        static MeshVertices* createCubeMesh();
        static MeshVertices* createSphereMesh(uint32_t latSegments, uint32_t lonSegments);
        static MeshVertices* createCapsuleMesh(float height, float radius);
        static MeshVertices* loadMeshAsset(const std::string& path);

        static std::string getTexturePath(const std::string& modelPath, const std::string& texturePath);
        static glm::mat4 getTransformFromAssimpTransform(const aiMatrix4x4& transform);
        static TextureWrap getTextureWrapFromAssimp(aiTextureMapMode mapMode);

        void traverseNodes(aiNode* node, const glm::mat4& parentTransform);

        VertexArrayObject* vao;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indexBuffer;
        std::vector<Submesh> submeshes;
        std::vector<std::unique_ptr<Material>> materials;
        std::vector<MeshNode> meshNodes{};
    };

}
