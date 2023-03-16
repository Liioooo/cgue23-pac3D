#pragma once

#include "Scripting/NativeScript.h"
#include "Logging.h"

using namespace CgEngine;

namespace Game {

    class CubeTestScript : public NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<CubeTestScript>();
        }

        void onCollisionEnter(Entity other) override {
            CG_LOGGING_INFO(other);
        }

        void onCollisionExit(Entity other) override {
            CG_LOGGING_INFO(other);
        }

        void onTriggerEnter(Entity other) override {
            CG_LOGGING_INFO("Trigger Enter: {0}", other);
        }

        void onTriggerExit(Entity other) override {
            CG_LOGGING_INFO("Trigger Exit: {0}", other);
        }
    };

}
