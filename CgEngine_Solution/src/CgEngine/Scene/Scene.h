#pragma once

#include <Rendering/SceneRenderer.h>
#include "Entity.h"
#include "ComponentManager.h"
#include "TimeStep.h"

namespace CgEngine {

    class Scene {
    public:
        Scene(int viewportWidth, int viewportHeight);
        ~Scene();

        Entity createEntity();
        Entity createEntity(const std::string& id);
        Entity createEntity(Entity parent);
        Entity createEntity(Entity parent, const std::string& id);
        void destroyEntity(Entity entity);
        Entity findEntityById(const std::string& id);
        const std::unordered_set<Entity>& getChildren(Entity entity);
        Entity getParent(Entity entity);
        bool hasEntity(Entity entity);
        void updateTransforms();
        void onViewportResize(int width, int height);
        int getViewportWidth() const;
        int getViewportHeight() const;
        void submitPostUpdateFunction(std::function<void()>&& function);
        void onUpdate(TimeStep ts);
        void onRender(SceneRenderer& renderer);

        template<typename C>
        C& attachComponent(Entity entity) {
            C comp = C(entity);
            componentManager->attachComponent(comp);
            C& addedComp = componentManager->getComponent<C>(entity);
            addedComp.onAttach(*this);
            return addedComp;
        }

        template<typename C, typename P>
        C& attachComponent(Entity entity, P componentParams) {
            C comp = C(entity);
            componentManager->attachComponent(comp);
            C& addedComp = componentManager->getComponent<C>(entity);
            addedComp.onAttach(*this, componentParams);
            return addedComp;
        }

        template<typename C>
        void detachComponent(Entity entity) {
            componentManager->detachComponent<C>(entity);
        }

        /**
         * References to Components must not be cached!!
         * Because if an other component of the same type is detached, the memory address of this component might change.
         * Which will lead to the reference pointing to a bad address!
         * @tparam C ComponentType
         * @param entity
         * @return reference to Component: auto& comp = s.getComponent<SomeComp>(entity);
         */
        template<typename C>
        C& getComponent(Entity entity) {
            return componentManager->getComponent<C>(entity);
        }

        template<typename C>
        bool hasComponent(Entity entity) const {
            return componentManager->hasComponent<C>(entity);
        }

        template<typename C>
        std::vector<Entity> getEntitiesWithComponent() {
            return componentManager->getEntitiesWithComponent<C>();
        }

        template<typename C>
        typename ComponentArray<C>::Iterator begin() {
            return componentManager->begin<C>();
        }

        template<typename C>
        typename ComponentArray<C>::Iterator end() {
            return componentManager->end<C>();
        }

        template<typename C>
        typename ComponentArray<C>::Iterator cbegin() const {
            return componentManager->cbegin<C>();
        }

        template<typename C>
        typename ComponentArray<C>::Iterator cend() const {
            return componentManager->cend<C>();
        }

        inline uint32_t getEntityCount() const {
            return entityCount;
        }

    private:
        Entity nextEntityId = 1;
        uint32_t entityCount = 0;
        std::unordered_map<std::string, Entity> idToEntity{};
        std::unordered_map<Entity, std::unordered_set<Entity>> children{};
        std::unordered_map<Entity, Entity> parents{};
        std::vector<std::function<void()>> postUpdateFunctions{};
        ComponentManager* componentManager = new ComponentManager();
        int viewportWidth;
        int viewportHeight;

        void recursiveDestroyEntity(Entity entity);
        void recursiveUpdateChildTransforms(Entity entity, const glm::mat4& parentModelMatrix, bool parentDirty);
        void executePostUpdateFunctions();
    };

}
