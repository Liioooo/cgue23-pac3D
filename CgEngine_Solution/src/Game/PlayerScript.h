#pragma once

#include <Scripting/NativeScript.h>
#include <Logging.h>

namespace Game {

    class PlayerScript : public CgEngine::NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<PlayerScript>();
        }

        void onAttach() override;
        void fixedUpdate() override;
        void update(CgEngine::TimeStep ts) override;
        void lateUpdate(CgEngine::TimeStep ts) override;

        void onMouseScrolled(CgEngine::MouseScrolledEvent& event) override;

    private:
        std::pair<float, float> prevMousePos;
        float pitch = 0;
        float yaw = 0;
        float cameraDistance = 4.0f;
    };

}
