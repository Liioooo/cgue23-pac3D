#include "PhysicsScene.h"
#include "GlobalObjectManager.h"
#include "Asserts.h"
#include "Scene/Scene.h"

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

        physXControllerManager = PxCreateControllerManager(*physXScene);
    }

    PhysicsScene::~PhysicsScene() {
        physXControllerManager->release();
        physXControllerManager = nullptr;

        physXScene->release();
        physXScene = nullptr;
    }

    void PhysicsScene::addActor(PhysicsActor* actor) {
        physXScene->addActor(actor->getPhysxActor());
    }

    void PhysicsScene::removeActor(PhysicsActor* actor) {
        physXScene->removeActor(actor->getPhysxActor());
    }

    PhysicsController* PhysicsScene::createController(Scene& scene, Entity entity, bool hasGravity, float stepOffset, float slopeLimit) {
        physx::PxController* physXController = nullptr;

        auto& transform = scene.getComponent<TransformComponent>(entity);

        if (scene.hasComponent<BoxColliderComponent>(entity)) {
            auto& collider = scene.getComponent<BoxColliderComponent>(entity);

            glm::vec3 scale = collider.getHalfSize() * transform.getGlobalScale();

            physx::PxBoxControllerDesc desc;
            desc.halfHeight = scale.y;
            desc.halfSideExtent = scale.x;
            desc.halfForwardExtent = scale.z;
            desc.position = PhysXUtils::glmToExtendedPhysXVec(transform.getGlobalPosition() + collider.getOffset());
            desc.upDirection = {0.0f, 1.0f, 0.0f};
            desc.stepOffset = stepOffset;
            desc.slopeLimit = glm::max(0.0f, glm::cos(glm::radians(slopeLimit)));
            desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;
            desc.contactOffset = 0.05f;
            desc.material = collider.getPhysicsMaterial().getPhysxMaterial();

            physXController = physXControllerManager->createController(desc);
        } else if (scene.hasComponent<CapsuleColliderComponent>(entity)) {
            auto& collider = scene.getComponent<CapsuleColliderComponent>(entity);

            physx::PxCapsuleControllerDesc desc;
            desc.height = collider.getHalfHeight() * 2.0f * transform.getGlobalScale().y;
            desc.radius = collider.getRadius() * glm::max(transform.getGlobalScale().x, transform.getGlobalScale().z);
            desc.position = PhysXUtils::glmToExtendedPhysXVec(transform.getGlobalPosition() + collider.getOffset());
            desc.upDirection = {0.0f, 1.0f, 0.0f};
            desc.stepOffset = stepOffset;
            desc.slopeLimit = glm::max(0.0f, glm::cos(glm::radians(slopeLimit)));
            desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;
            desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
            desc.contactOffset = 0.05f;
            desc.material = collider.getPhysicsMaterial().getPhysxMaterial();

            physXController = physXControllerManager->createController(desc);
        }

        CG_ASSERT(physXController != nullptr, "Cannot create PhysicsController! Entity using Controller must have: BoxColliderComponent or CapsuleColliderComponent")

        return new PhysicsController(physXController, hasGravity, entity);
    }

    void PhysicsScene::simulate(TimeStep ts, Scene& scene) {
        bool hasSimulated = advance(ts.getSeconds(), scene);

        if (hasSimulated) {
            uint32_t actorCount;
            physx::PxActor** activeActors = physXScene->getActiveActors(actorCount);

            for(uint32_t i = 0; i < actorCount; i++) {
                auto* actor = static_cast<PhysicsActor*>(activeActors[i]->userData);
                if (actor && !actor->isSleeping()) {
                    actor->updateTransforms();
                }
            }

            for (uint32_t i = 0; i < physXControllerManager->getNbControllers(); i++) {
                static_cast<PhysicsController*>(physXControllerManager->getController(i)->getUserData())->updateTransforms(scene);
            }

        }
    }

    bool PhysicsScene::advance(float ts, Scene& scene) {

        accumulator += ts;

        if (accumulator < simulateTimeStep) {
            return false;
        }

        uint32_t numSteps = glm::min(static_cast<uint32_t>(accumulator / simulateTimeStep), 4u);

        for (uint32_t i = 0; i < numSteps; i++) {
            accumulator -= simulateTimeStep;
            physXScene->simulate(simulateTimeStep);
            physXScene->fetchResults(true);
            scene.executeFixedUpdate();
            updateControllers(simulateTimeStep);
        }
        return true;
    }

    void PhysicsScene::updateControllers(float ts) {
        for (uint32_t i = 0; i < physXControllerManager->getNbControllers(); i++) {
            static_cast<PhysicsController*>(physXControllerManager->getController(i)->getUserData())->update(ts);
        }
    }
}
