#include "ScriptComponent.h"
#include "GlobalObjectManager.h"

namespace CgEngine {
    void ScriptComponent::onAttach(Scene &scene, ScriptComponentParams &params) {
        script = GlobalObjectManager::getInstance().getScriptManager().getScriptInstance(params.scriptName);
        script->setEntity(entity);
        script->setScene(&scene);
        script->onAttach();
    }

    void ScriptComponent::onDetach() {
        script->onDetach();
    }

    void ScriptComponent::update() {
        script->update();
    }

    void ScriptComponent::lateUpdate() {
        script->lateUpdate();
    }
}
