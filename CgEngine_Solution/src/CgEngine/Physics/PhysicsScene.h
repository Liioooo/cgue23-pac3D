#pragma once

#include "Scene/Entity.h"
#include "TimeStep.h"
#include "PhysicsActor.h"
#include "PxPhysicsAPI.h"
#include "PhysicsController.h"

namespace CgEngine {

    class Scene;

    class PhysicsScene {
    public:
        PhysicsScene();
        ~PhysicsScene();

        void addActor(PhysicsActor* actor);
        void removeActor(PhysicsActor* actor);
        PhysicsController* createController(Scene& scene, Entity entity, bool hasGravity, float stepOffset, float slopeLimit);
        void simulate(TimeStep ts, Scene& scene);

    private:
        physx::PxScene* physXScene;
        physx::PxControllerManager* physXControllerManager;

        bool advance(float ts, Scene& scene);
        void updateControllers(float ts);

        float simulateTimeStep;
        float accumulator;
    };

}
