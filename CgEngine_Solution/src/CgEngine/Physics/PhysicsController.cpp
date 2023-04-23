#include "PhysicsController.h"
#include "PhysicsSystem.h"
#include "Scene/Scene.h"
#include "GlobalObjectManager.h"

namespace CgEngine {
    PhysicsController::PhysicsController(physx::PxController* physXController, bool hasGravity, Entity entity, Scene& scene) : AbstractPhysicsActor(&scene, entity), physXController(physXController), hasGravity(hasGravity), entity(entity) {
        gravity = GlobalObjectManager::getInstance().getPhysicsSystem().getPhysxSettings().gravity;
        physXController->setUserData(this);
        physXController->getActor()->userData = this;
    }

    PhysicsController::~PhysicsController() {
        physXController->release();
    }

    PhysicsActorType PhysicsController::getPhysicsActorType() const {
        return PhysicsActorType::Controller;
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

    void PhysicsController::updateTransforms() {
        auto& transformComp = getScene().getComponent<TransformComponent>(entity);
        auto pos = PhysXUtils::phsXExtendedToGlmVec(physXController->getPosition());

        if (getScene().hasComponent<BoxColliderComponent>(entity)) {
            pos -= getScene().getComponent<BoxColliderComponent>(entity).getOffset();
        }
        if (getScene().hasComponent<CapsuleColliderComponent>(entity)) {
            pos -= getScene().getComponent<CapsuleColliderComponent>(entity).getOffset();
        }

        transformComp._physicsUpdate(pos, transformComp.getGlobalRotationQuat());
    }

    void PhysicsController::move(glm::vec3 dir) {
        currentMovement += dir;
    }

    void PhysicsController::jump(float strength) {
        currentJumpSpeed = strength;
    }

    bool PhysicsController::standsOnGround() const {
        return collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
    }
}