#pragma once

#include "Component.h"

namespace CgEngine {

    struct DirectionalLightComponentParams {
        glm::vec3 color;
        float intensity;
        bool castShadows;
    };

    class DirectionalLightComponent : public Component {
    public:
        using Component::Component;

        void onAttach(Scene& scene, DirectionalLightComponentParams& params);

        glm::vec3 getColor();
        void setColor(const glm::vec3& value);
        float getIntensity();
        void setIntensity(float value);
        bool getCastShadows();
        void setCastShadows(bool value);

    private:
        glm::vec3 color;
        float intensity;
        bool castShadows;
    };

}
