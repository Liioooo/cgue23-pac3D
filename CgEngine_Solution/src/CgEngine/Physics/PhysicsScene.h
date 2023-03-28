#pragma once

#include "Scene/Entity.h"
#include "TimeStep.h"
#include "PhysicsActor.h"
#include "PxPhysicsAPI.h"

namespace CgEngine {

    class Scene;

    class PhysicsScene {
    public:
        PhysicsScene();
        ~PhysicsScene();

        void addActor(PhysicsActor* actor);
        void removeActor(PhysicsActor* actor);
        void simulate(TimeStep ts, Scene& scene);

    private:
        physx::PxScene* physXScene;
        std::unordered_map<Entity, PhysicsActor> actors;

        bool advance(float ts, Scene& scene);

        float simulateTimeStep;
        float accumulator;
    };

}
