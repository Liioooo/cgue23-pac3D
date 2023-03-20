#include <numeric>
#include "MeshRendererComponent.h"
#include "GlobalObjectManager.h"

namespace CgEngine {
    void MeshRendererComponent::onAttach(Scene& scene, MeshRendererComponentParams &params) {
        if (!params.mesh.empty()) {
            mesh = GlobalObjectManager::getInstance().getResourceManager().getResource<MeshVertices>(params.mesh);
            submeshIndices.push_back(0);
        } else {
            mesh = GlobalObjectManager::getInstance().getResourceManager().getResource<MeshVertices>(params.assetFile);
            if (params.submeshIndices.empty()) {
                submeshIndices = std::vector<uint32_t>(mesh->getSubmeshes().size());
                std::iota(submeshIndices.begin(), submeshIndices.end(), 0);
            } else {
                submeshIndices = params.submeshIndices;
            }
        }

        if (params.material.empty()) {
            material = nullptr;
        } else {
            material = GlobalObjectManager::getInstance().getResourceManager().getResource<Material>(params.material);
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
