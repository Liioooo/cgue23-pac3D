#pragma once

#include <Resources/MeshVertices.h>
#include <Rendering/Material.h>
#include "Component.h"

namespace CgEngine {

    struct MeshRendererComponentParams {
        std::string assetFile;
        std::string mesh;
        std::string material;
        std::vector<uint32_t> submeshIndices;
    };

    class MeshRendererComponent : public Component {
    public:
        using Component::Component;

        void onAttach(Scene& scene, MeshRendererComponentParams& params);

        MeshVertices& getMeshVertices();
        Material* getMaterial();
        const std::vector<uint32_t>& getSubmeshIndices();

    private:
        MeshVertices* mesh;
        Material* material;
        std::vector<uint32_t> submeshIndices;
    };

}
