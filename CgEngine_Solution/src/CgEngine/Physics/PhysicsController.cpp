#include "PhysicsController.h"
#include "Scene/Scene.h"

namespace CgEngine {
    PhysicsController::PhysicsController(physx::PxController* physXController, bool hasGravity, glm::vec3 gravity, Entity entity, Scene& scene) : AbstractPhysicsActor(&scene, entity), physXController(physXController), hasGravity(hasGravity), gravity(gravity), entity(entity) {
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

        if (shouldTeleport) {
            physXController->setPosition(PhysXUtils::glmToExtendedPhysXVec(desiredPosition));
            shouldTeleport = false;
        } else {
            physx::PxControllerFilters filters;

            if (hasGravity && currentJumpSpeed <= 0) {
                currentMovement += gravity * ts;
            }

            if (currentJumpSpeed > 0) {
                currentJumpSpeed += gravity.y * ts;
                if (currentJumpSpeed < 0) {
                    currentJumpSpeed = 0.0f;
                }
            }

            currentMovement.y += currentJumpSpeed;

            collisionFlags = physXController->move(PhysXUtils::glmToPhysXVec(currentMovement), 0.0f, ts, filters);
        }

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

    void PhysicsController::setPosition(glm::vec3 pos) {
        desiredPosition = pos;
        shouldTeleport = true;
    }

    void PhysicsController::jump(float strength) {
        currentJumpSpeed = strength;
    }

    bool PhysicsController::standsOnGround() const {
        return collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN;
    }
}