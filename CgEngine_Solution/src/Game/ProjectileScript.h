#pragma once

#include "Scripting/NativeScript.h"

namespace Game {

    class ProjectileScript : public CgEngine::NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<ProjectileScript>();
        }

    protected:
        void update(CgEngine::TimeStep ts) override;

        void onCollisionEnter(CgEngine::Entity other) override;

    private:
        float lifetime = 5.0f;

    };

}
