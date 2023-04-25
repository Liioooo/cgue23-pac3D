#include "PhysicsController.h"
#include "Scene/Scene.h"
#include "PhysicsScene.h"

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

            if (grounded && velocity.y < 0.0f) {
                velocity.y = 0.0f;
            }

            updateIsGrounded();

            if (nextJumpStrength != 0.0f) {
                velocity += glm::sqrt(nextJumpStrength * -3.0f * gravity);
                nextJumpStrength = 0.0f;
                grounded = false;
            }

            if (glm::length(movement) != 0.0f || !grounded) {
                velocity += gravity * ts;
                movement.y += velocity.y * ts;

                physx::PxControllerCollisionFlags collisionFlags = physXController->move(PhysXUtils::glmToPhysXVec(movement), 0.001f, ts, filters);
                if (collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN) {
                    grounded = true;
                }
            }

        }

        movement = {0.0f, 0.0f, 0.0f};
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
        movement += dir;
    }

    void PhysicsController::setPosition(glm::vec3 pos) {
        desiredPosition = pos;
        shouldTeleport = true;
    }

    void PhysicsController::jump(float strength) {
        nextJumpStrength = strength;
    }

    bool PhysicsController::isGrounded() const {
        return grounded;
    }

    void PhysicsController::updateIsGrounded() {
        auto* physicsScene = static_cast<PhysicsScene*>(physXController->getScene()->userData);

        auto raycastHit = physicsScene->raycast(PhysXUtils::phsXExtendedToGlmVec(physXController->getFootPosition()), glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), 1000.0f, std::unordered_set<Entity>{});
        grounded = raycastHit.hitFound && ((grounded && raycastHit.hitDistance < 0.05f) || (raycastHit.hitDistance < 0.001f));
    }
}