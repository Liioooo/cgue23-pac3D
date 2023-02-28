#include "MeshRendererComponent.h"
#include "GlobalObjectManager.h"

namespace CgEngine {
    void MeshRendererComponent::onAttach(Scene& scene, MeshRendererComponentParams &params) {
        if (!params.mesh.empty()) {
            mesh = GlobalObjectManager::getInstance().getResourceManager().getResource<MeshVertices>(params.mesh);
            material = GlobalObjectManager::getInstance().getResourceManager().getResource<Material>(params.material);
            submeshIndices.push_back(0);
        } else {
            // TODO: get MeshVert from 3D-Model, get Material from 3d-Model
            // + Override Material
        }
    }

    MeshVertices &MeshRendererComponent::getMeshVertices() {
        return *mesh;
    }

    Material* MeshRendererComponent::getMaterial() {
        return material;
    }

    const std::vector<uint32_t> &MeshRendererComponent::getSubmeshIndices() {
        return submeshIndices;
    }
}
