#pragma once

#include "Scripting/NativeScript.h"

namespace Game {

    class LostScript : public CgEngine::NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<LostScript>();
        }

    protected:
        void onAttach() override;

        void onMouseButtonPressed(CgEngine::MouseButtonPressedEvent& event) override;

    };

}
