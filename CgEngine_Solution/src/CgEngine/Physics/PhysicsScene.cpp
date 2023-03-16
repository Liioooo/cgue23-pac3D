#include "PhysicsScene.h"
#include "GlobalObjectManager.h"
#include "Asserts.h"


namespace CgEngine {
    PhysicsScene::PhysicsScene() {
        auto& physicsSystem = GlobalObjectManager::getInstance().getPhysicsSystem();

        simulateTimeStep = physicsSystem.getPhysxSettings().simulateTimeStep;

        physx::PxSceneDesc sceneDesc(physicsSystem.getPhysxPhysics().getTolerancesScale());
        sceneDesc.gravity = PhysXUtils::glmToPhysXVec(physicsSystem.getPhysxSettings().gravity);
        sceneDesc.cpuDispatcher = &physicsSystem.getPhysxCpuDispatcher();
        sceneDesc.simulationEventCallback = &physicsSystem.getSimulationEventCallbacks();
        sceneDesc.broadPhaseType = physx::PxBroadPhaseType::ePABP;
        sceneDesc.frictionType = physx::PxFrictionType::ePATCH;
        sceneDesc.filterShader = PhysicsSystem::filterShader;
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;
        sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS;


        CG_ASSERT(sceneDesc.isValid(), "PhysX SceneDesc is invalid!")

        physXScene = physicsSystem.getPhysxPhysics().createScene(sceneDesc);
        CG_ASSERT(physXScene != nullptr, "Error creating PhysXScene!")
    }

    PhysicsScene::~PhysicsScene() {
        physXScene->release();
        physXScene = nullptr;
    }

    void PhysicsScene::addActor(PhysicsActor* actor) {
        physXScene->addActor(actor->getPhysxActor());
    }

    void PhysicsScene::removeActor(PhysicsActor* actor) {
        physXScene->removeActor(actor->getPhysxActor());
    }

    void PhysicsScene::simulate(TimeStep ts) {
        bool hasSimulated = advance(ts.getSeconds());

        if (hasSimulated) {
            uint32_t actorCount;
            physx::PxActor** activeActors = physXScene->getActiveActors(actorCount);

            for(uint32_t i = 0; i < actorCount; i++) {
                auto* actor = static_cast<PhysicsActor*>(activeActors[i]->userData);
                if (actor && !actor->isSleeping()) {
                    actor->updateTransforms();
                }
            }
        }
    }

    bool PhysicsScene::advance(float ts) {

        accumulator += ts;

        if (accumulator < simulateTimeStep) {
            return false;
        }

        uint32_t numSteps = glm::min(static_cast<uint32_t>(accumulator / simulateTimeStep), 4u);

        for (uint32_t i = 0; i < numSteps; i++) {
            accumulator -= simulateTimeStep;
            physXScene->simulate(simulateTimeStep);
            physXScene->fetchResults(true);
        }
        return true;
    }
}
