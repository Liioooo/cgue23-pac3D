#pragma once

#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace CgEngine {

    class NativeScript {
    public:
        NativeScript() = default;
        ~NativeScript() = default;

        void setScene(Scene* scene);
        void setEntity(Entity entity);

        virtual void onAttach() {};
        virtual void onDetach() {};

        virtual void update() {};
        virtual void lateUpdate() {};


    protected:
        Entity getOwingEntity();
        void destroyEntity(Entity entity);
        Entity findEntityById(const std::string& id);
        Entity getParentEntity();
        const std::unordered_set<Entity>& getChildEntities();

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
        bool attachComponent() {
            return owningScene->attachComponent<C>(owningEntity);
        }

        template<typename C, typename P>
        bool attachComponent(P componentPrams) {
            return owningScene->attachComponent<C>(owningEntity, componentPrams);
        }

        template<typename C>
        bool detachComponent() {
            return owningScene->detachComponent<C>(owningEntity);
        }

    private:
        Scene* owningScene;
        Entity owningEntity;
    };

}
