#include "NativeScript.h"
#include "Scene/Scene.h"

namespace CgEngine {
    void NativeScript::_setScene(Scene* scene) {
        owningScene = scene;
    }

    void NativeScript::_setEntity(Entity entity) {
        owningEntity = entity;
    }

    Entity NativeScript::getOwingEntity() {
        return owningEntity;
    }

    void NativeScript::destroyEntity(Entity entity) {
        auto& scene = owningScene;

        scene->submitPostUpdateFunction([entity, scene]() {
            scene->destroyEntity(entity);
        });
    }

    Entity NativeScript::findEntityById(const std::string &id) {
        return owningScene->findEntityById(id);
    }

    Entity NativeScript::getParentEntity() {
        return owningScene->getParent(owningEntity);
    }

    const std::unordered_set<Entity>& NativeScript::getChildEntities() {
        return owningScene->getChildren(owningEntity);
    }
}
