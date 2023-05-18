#pragma once

#include "Scripting/NativeScript.h"

namespace Game {

    class StartScript : public CgEngine::NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<StartScript>();
        }

    protected:
        void onAttach() override;

        void onMouseButtonPressed(CgEngine::MouseButtonPressedEvent& event) override;
        void onMouseMoved(CgEngine::MouseMovedEvent& event) override;

    };

}
