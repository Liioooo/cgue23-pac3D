#include "ScriptComponent.h"
#include "GlobalObjectManager.h"

namespace CgEngine {
    void ScriptComponent::onAttach(Scene &scene, ScriptComponentParams &params) {
        script = GlobalObjectManager::getInstance().getScriptManager().getScriptInstance(params.scriptName);
        script->_setEntity(entity);
        script->_setScene(&scene);
        script->onAttach();
    }

    void ScriptComponent::onDetach(Scene& scene) {
        script->onDetach();
    }

    void ScriptComponent::fixedUpdate() {
        script->fixedUpdate();
    }

    void ScriptComponent::update(TimeStep ts) {
        script->update(ts);
    }

    void ScriptComponent::lateUpdate(TimeStep ts) {
        script->lateUpdate(ts);
    }

    void ScriptComponent::onCollisionEnter(Entity other) {
        script->onCollisionEnter(other);
    }

    void ScriptComponent::onCollisionExit(Entity other) {
        script->onCollisionExit(other);
    }

    void ScriptComponent::onTriggerEnter(Entity other) {
        script->onTriggerEnter(other);
    }

    void ScriptComponent::onTriggerExit(Entity other) {
        script->onTriggerExit(other);
    }
}
