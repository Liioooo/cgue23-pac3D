#include "NativeScript.h"
#include "Scene/Scene.h"
#include "Rendering/SceneRenderer.h"

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

    Entity NativeScript::findEntityById(const std::string& id) {
        return owningScene->findEntityById(id);
    }

    Entity NativeScript::getParentEntity() {
        return owningScene->getParent(owningEntity);
    }

    const std::unordered_set<Entity>& NativeScript::getChildEntities() {
        return owningScene->getChildren(owningEntity);
    }

    void NativeScript::drawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color) {
        owningScene->submitOnRenderFunction([from, to, color](SceneRenderer& renderer) {
            renderer.submitDebugLine(from, to, color);
        });
    }
}
