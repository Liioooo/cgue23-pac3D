#pragma once

#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace CgEngine {

    class NativeScript {
    public:
        NativeScript() = default;
        ~NativeScript() = default;

        void _setScene(Scene* scene);
        void _setEntity(Entity entity);

        virtual void onAttach() {};
        virtual void onDetach() {};

        virtual void fixedUpdate() {};
        virtual void update(TimeStep ts) {};
        virtual void lateUpdate(TimeStep ts) {};

        virtual void onCollisionEnter(Entity other) {}
        virtual void onCollisionExit(Entity other) {}

        virtual void onTriggerEnter(Entity other) {};
        virtual void onTriggerExit(Entity other) {};


    protected:
        Entity getOwingEntity();
        void destroyEntity(Entity entity);
        Entity findEntityById(const std::string& id);
        Entity getParentEntity();
        const std::unordered_set<Entity>& getChildEntities();

        void drawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);

        template<typename C>
        C& getComponent() {
            return owningScene->getComponent<C>(owningEntity);
        }

        template<typename C>
        C& getComponent(Entity entity) {
            return owningScene->getComponent<C>(entity);
        }

        template<typename C>
        bool hasEntityComponent() {
            return owningScene->hasComponent<C>(owningEntity);
        }

        template<typename C>
        bool hasEntityComponent(Entity entity) {
            return owningScene->hasComponent<C>(entity);
        }

        template<typename C>
        C& attachComponent() {
            return owningScene->attachComponent<C>(owningEntity);
        }

        template<typename C, typename P>
        C& attachComponent(P componentPrams) {
            return owningScene->attachComponent<C>(owningEntity, componentPrams);
        }

        template<typename C>
        void detachComponent() {
            owningScene->detachComponent<C>(owningEntity);
        }

    private:
        Scene* owningScene;
        Entity owningEntity;
    };

}
