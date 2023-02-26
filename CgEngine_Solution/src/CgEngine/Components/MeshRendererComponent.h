#pragma once

#include <Resources/MeshVertices.h>
#include <Rendering/Material.h>
#include "Component.h"

namespace CgEngine {

    struct MeshRendererComponentParams {
        std::string mesh;
        std::string material;
    };

    class MeshRendererComponent : public Component {
    public:
        using Component::Component;

        void onAttach(Scene& scene, MeshRendererComponentParams& params);

        MeshVertices& getMeshVertices();
        Material& getMaterial();

    private:
        MeshVertices* mesh;
        Material* material;
    };

}
