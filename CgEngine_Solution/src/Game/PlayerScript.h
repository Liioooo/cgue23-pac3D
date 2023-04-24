#pragma once

#include "Scripting/NativeScript.h"
#include "Logging.h"

namespace Game {

    class PlayerScript : public CgEngine::NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<PlayerScript>();
        }

        void onAttach() override;
        void fixedUpdate(CgEngine::TimeStep ts) override;
        void update(CgEngine::TimeStep ts) override;
        void lateUpdate(CgEngine::TimeStep ts) override;

        void onTriggerEnter(CgEngine::Entity other) override;
        void onCollisionEnter(CgEngine::Entity other) override;

        void onMouseScrolled(CgEngine::MouseScrolledEvent& event) override;
        void onKeyPressed(CgEngine::KeyPressedEvent& event) override;

    private:
        std::pair<float, float> prevMousePos;
        float pitch = -0.6f;
        float yaw = 0.0f;
        float cameraDistance = 4.0f;
        glm::vec3 cameraOffset = glm::vec3(0.0f, 1.5f, 0.0f);
        std::unordered_set<CgEngine::Entity> cameraRaycastExcluded;
    };

}
