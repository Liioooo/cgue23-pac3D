#pragma once

#include "Events/Event.h"
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

        void fixedUpdate();
        void update(TimeStep ts);
        void lateUpdate(TimeStep ts);

        void onCollisionEnter(Entity other);
        void onCollisionExit(Entity other);

        void onTriggerEnter(Entity other);
        void onTriggerExit(Entity other);

        void onEvent(Event& event);

    private:
        std::shared_ptr<NativeScript> script;
    };

}


