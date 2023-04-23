#include "PhysicsController.h"
#include "PhysicsSystem.h"
#include "Scene/Scene.h"
#include "GlobalObjectManager.h"

namespace CgEngine {
    PhysicsController::PhysicsController(physx::PxController* physXController, bool hasGravity, Entity entity) : physXController(physXController), hasGravity(hasGravity), entity(entity) {
        gravity = GlobalObjectManager::getInstance().getPhysicsSystem().getPhysxSettings().gravity;
        physXController->setUserData(this);
    }

    PhysicsController::~PhysicsController() {
        physXController->release();
    }

    void PhysicsController::update(float ts) {
        physx::PxControllerFilters filters;

        if (hasGravity && !standsOnGround() && currentJumpSpeed <= 0) {
            currentMovement += gravity * ts;
        }

        if (currentJumpSpeed > 0) {
            currentJumpSpeed += gravity.y * ts;
        }

        currentMovement.y += currentJumpSpeed;

        collisionFlags = physXController->move(PhysXUtils::glmToPhysXVec(currentMovement), 0.0f, ts, filters);

        currentMovement = {0.0f, 0.0f, 0.0f};
    }

    void PhysicsController::updateTransforms(CgEngine::Scene& scene) {
        auto& transformComp = scene.getComponent<TransformComponent>(entity);
        auto pos = PhysXUtils::phsXExtendedToGlmVec(physXController->getPosition());

        if (scene.hasComponent<BoxColliderComponent>(entity)) {
            pos -= scene.getComponent<BoxColliderComponent>(entity).getOffset();
        }
        if (scene.hasComponent<CapsuleColliderComponent>(entity)) {
            pos -= scene.getComponent<CapsuleColliderComponent>(entity).getOffset();
        }

        transformComp._physicsUpdate(pos, transformComp.getGlobalRotationQuat());
    }

    void PhysicsController::move(glm::vec3 dir) {
        currentMovement += dir;
    }

    void PhysicsController::jump(float strength) {
        currentJumpSpeed = strength;
    }

    bool PhysicsController::standsOnGround() {
        return collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
    }
}