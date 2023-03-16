#pragma once

#include "Component.h"
#include "TimeStep.h"

namespace CgEngine {

    class NativeScript;

    struct ScriptComponentParams {
        std::string scriptName;
    };

    class ScriptComponent : public Component {
    public:
        using Component::Component;

        void onAttach(Scene& scene, ScriptComponentParams& params);
        void onDetach(Scene& scene) override;

        void update(TimeStep ts);
        void lateUpdate(TimeStep ts);

        void onCollisionEnter(Entity other);
        void onCollisionExit(Entity other);

        void onTriggerEnter(Entity other);
        void onTriggerExit(Entity other);

    private:
        std::shared_ptr<NativeScript> script;
    };

}


