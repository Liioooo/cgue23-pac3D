#pragma once

#include "Scripting/NativeScript.h"

namespace Game {

    class SingleGhostScript : public CgEngine::NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<SingleGhostScript>();
        }

        void onCollisionEnter(CgEngine::Entity other) override;

    };

}
