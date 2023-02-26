#include "Scene.h"
#include "Asserts.h"

namespace CgEngine {
    Scene::Scene(int viewportWidth, int viewportHeight) : viewportWidth(viewportWidth), viewportHeight(viewportHeight) {}

    Scene::~Scene() {
        delete componentManager;
    }

    Entity Scene::createEntity() {
        entityCount++;
        Entity entity = nextEntityId++;
        children[entity] = std::unordered_set<Entity>();
        return entity;
    }

    Entity Scene::createEntity(const std::string &id) {
        if (idToEntity.count(id) != 0) {
            CG_ASSERT(false, "Entity-Id is already used")
            return CG_ENTITY_UNAVAILABLE;
        }

        entityCount++;
        Entity entity = nextEntityId++;
        children[entity] = std::unordered_set<Entity>();
        idToEntity[id] = entity;
        return entity;
    }

    Entity Scene::createEntity(Entity parent) {
        if (children.count(parent) == 0) {
            CG_ASSERT(false, "Entity parent doesn't exist")
            return CG_ENTITY_UNAVAILABLE;
        }

        entityCount++;
        Entity entity = nextEntityId++;
        children[parent].insert(entity);
        children[entity] = std::unordered_set<Entity>();
        parents[entity] = parent;
        return entity;

    }

    Entity Scene::createEntity(Entity parent, const std::string &id) {
        if (children.count(parent) == 0) {
            CG_ASSERT(false, "Entity parent doesn't exist")
            return CG_ENTITY_UNAVAILABLE;
        }
        if (idToEntity.count(id) != 0) {
            CG_ASSERT(false, "Entity-Id is already used")
            return CG_ENTITY_UNAVAILABLE;
        }

        entityCount++;
        Entity entity = nextEntityId++;
        idToEntity[id] = entity;
        children[parent].insert(entity);
        children[entity] = std::unordered_set<Entity>();
        parents[entity] = parent;
        return entity;
    }

    void Scene::destroyEntity(Entity entity) {
        if (parents.count(entity) != 0) {
            children[parents[entity]].erase(entity);
        }
        recursiveDestroyEntity(entity);
    }

    Entity Scene::findEntityById(const std::string &id) {
        if (idToEntity.count(id) == 0) {
            return CG_ENTITY_UNAVAILABLE;
        }
        return idToEntity[id];
    }

    const std::unordered_set<Entity>& Scene::getChildren(Entity entity) {
        return children[entity];
    }

    Entity Scene::getParent(Entity entity) {
        if (parents.count(entity) == 0) {
            return CG_ENTITY_UNAVAILABLE;
        }
        return parents[entity];
    }

    bool Scene::hasEntity(Entity entity) {
        return children.count(entity) != 0;
    }

    void Scene::updateTransforms() {
        for (const auto &item : children) {
            if (parents.count(item.first) == 0) {
                auto& topLevelTransform = componentManager->getComponent<TransformComponent>(item.first);
                bool topLevelDirty = topLevelTransform._calculateTopLevelTransforms();
                const glm::mat4& topLevelModelMatrix = topLevelTransform.getModelMatrix();

                for (const auto &child : children[item.first]) {
                    recursiveUpdateChildTransforms(child, topLevelModelMatrix, topLevelDirty);
                }
            }
        }
    }

    void Scene::onViewportResize(int width, int height) {
        if (width == viewportWidth && height == viewportHeight) {
            return;
        }
        viewportWidth = width;
        viewportHeight = height;

        auto cameras = componentManager->getEntitiesWithComponent<CameraComponent>();
        for (const auto &item: cameras) {
            auto& camaraComp = getComponent<CameraComponent>(item);
            camaraComp.getCamera().setViewportSize(width, height);
        }
    }

    void Scene::submitPostUpdateFunction(std::function<void()>&& function) {
        postUpdateFunctions.emplace_back(function);
    }

    void Scene::onUpdate() {
        for (auto it = componentManager->begin<ScriptComponent>(); it != componentManager->end<ScriptComponent>(); it++) {
            it->update();
        }
        executePostUpdateFunctions();
        updateTransforms();
        for (auto it = componentManager->begin<ScriptComponent>(); it != componentManager->end<ScriptComponent>(); it++) {
            it->lateUpdate();
        }
        executePostUpdateFunctions();
        updateTransforms();
    }

    void Scene::onRender(SceneRenderer& renderer) {
        auto cameraComponent = std::find_if(componentManager->begin<CameraComponent>(), componentManager->end<CameraComponent>(), [](auto&& c) { return c.isPrimary();});
        auto cameraTransform = componentManager->getComponent<TransformComponent>(cameraComponent->getEntity());

        renderer.setActiveScene(this);
        renderer.beginScene(cameraComponent->getCamera(), cameraTransform.getModelMatrix());

        for (auto it = componentManager->begin<MeshRendererComponent>(); it != componentManager->end<MeshRendererComponent>(); it++) {
            renderer.submitMesh(*it->getMeshVertices().getVAO(), it->getMaterial(), componentManager->getComponent<TransformComponent>(it->getEntity()).getModelMatrix());
        }

        renderer.endScene();
    }

    int Scene::getViewportWidth() const {
        return viewportWidth;
    }

    int Scene::getViewportHeight() const {
        return viewportHeight;
    }

    void Scene::recursiveDestroyEntity(Entity entity) {
        for (const auto &child : children[entity]) {
            recursiveDestroyEntity(child);
        }

        auto idIter = std::find_if(
                std::begin(idToEntity),
                std::end(idToEntity),
                [entity](auto&& p) { return p.second == entity;}
        );
        if (idIter != std::end(idToEntity)) {
            idToEntity.erase(idIter->first);
        }

        componentManager->destroyEntity(entity);
        parents.erase(entity);
        children.erase(entity);
        entityCount--;
    }

    void Scene::recursiveUpdateChildTransforms(Entity entity, const glm::mat4& parentModelMatrix, bool parentDirty) {
        auto& transform = componentManager->getComponent<TransformComponent>(entity);
        bool dirty = transform._calculateChildTransformsWithParent(parentModelMatrix, parentDirty);

        for (const auto &child: children[entity]) {
            recursiveUpdateChildTransforms(child, transform.getModelMatrix(), dirty);
        }
    }

    void Scene::executePostUpdateFunctions() {
        for (const auto &fn: postUpdateFunctions) {
            fn();
        }
        postUpdateFunctions.clear();
    }
}
