#pragma once

#include "Component.h"

namespace CgEngine {

    class NativeScript;

    struct ScriptComponentParams {
        std::string scriptName;
    };

    class ScriptComponent : public Component {
    public:
        using Component::Component;

        void onAttach(Scene& scene, ScriptComponentParams& params);
        void onDetach() override;

        void update();
        void lateUpdate();

    private:
        std::shared_ptr<NativeScript> script;
    };

}


