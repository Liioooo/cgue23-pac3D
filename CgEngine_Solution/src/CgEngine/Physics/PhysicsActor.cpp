#include "PhysicsActor.h"
#include "GlobalObjectManager.h"
#include "PhysicsShapeBox.h"
#include "PhysicsShapeSphere.h"
#include "Uuid.h"

namespace CgEngine {
    PhysicsActor::PhysicsActor(Scene* scene, Entity entity, glm::vec3 pos, glm::quat orientation, bool isDynamic, PhysicsCollisionDetection collisionDetection) : scene(scene), entity(entity), dynamic(isDynamic) {
        auto& physicsSystem = GlobalObjectManager::getInstance().getPhysicsSystem();

        physx::PxTransform transform(PhysXUtils::glmToPhysXVec(pos), PhysXUtils::glmToPhysXQuat(orientation));

        if (!isDynamic) {
            physxActor = physicsSystem.getPhysxPhysics().createRigidStatic(transform);
        } else {
            physxActor = physicsSystem.getPhysxPhysics().createRigidDynamic(transform);

            physxActor->is<physx::PxRigidDynamic>()->setSolverIterationCounts(physicsSystem.getPhysxSettings().solverIterations, physicsSystem.getPhysxSettings().solverVelocityIterations);
            physxActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, collisionDetection == PhysicsCollisionDetection::Continuous);

            filterData.word0 = static_cast<uint32_t>(collisionDetection);
        }

        physxActor->userData = this;
    }

    PhysicsActor::~PhysicsActor() {
        for (const auto& item: colliders) {
            item.second->detachFromActor(physxActor);
            delete item.second;
        }

        physxActor->release();
        physxActor = nullptr;
    }

    physx::PxRigidActor& PhysicsActor::getPhysxActor() const {
        return *physxActor;
    }

    const physx::PxFilterData& PhysicsActor::getFilterData() const {
        return filterData;
    }

    Scene& PhysicsActor::getScene() const {
        return *scene;
    }

    Entity PhysicsActor::getEntity() const {
        return entity;
    }

    bool PhysicsActor::isSleeping() {
        return dynamic && physxActor->is<physx::PxRigidDynamic>()->isSleeping();
    }

    void PhysicsActor::updateTransforms() {
        auto& transformComp = scene->getComponent<TransformComponent>(entity);
        physx::PxTransform transform = physxActor->getGlobalPose();
        transformComp._physicsUpdate(PhysXUtils::phsXToGlmVec(transform.p), PhysXUtils::phsXToGlmQuat(transform.q));
    }

    void PhysicsActor::setKinematic(bool isKinematic) {
        if (!dynamic) {
            return;
        }

        kinematic = isKinematic;
        physxActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematic);
    }

    void PhysicsActor::setMass(float mass) {
        if (!dynamic) {
            return;
        }

        auto* actor = physxActor->is<physx::PxRigidDynamic>();
        physx::PxRigidBodyExt::setMassAndUpdateInertia(*actor, mass);
    }

    void PhysicsActor::setAngularDrag(float drag) {
        if (!dynamic) {
            return;
        }

        auto* actor = physxActor->is<physx::PxRigidDynamic>();
        actor->setAngularDamping(drag);
    }

    void PhysicsActor::setLinearDrag(float drag) {
        if (!dynamic) {
            return;
        }

        auto* actor = physxActor->is<physx::PxRigidDynamic>();
        actor->setLinearDamping(drag);
    }

    void PhysicsActor::setGravityDisabled(bool disabled) {
        if (!dynamic) {
            return;
        }

        physxActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, disabled);
    }

    bool PhysicsActor::isDynamic() const {
        return dynamic;
    }

    bool PhysicsActor::isKinematic() const {
        return kinematic;
    }

    void PhysicsActor::addForce(glm::vec3 force, PhysicsForceMode forceMode) {
        if (!dynamic || kinematic) {
            return;
        }

        physxActor->is<physx::PxRigidDynamic>()->addForce(PhysXUtils::glmToPhysXVec(force), static_cast<physx::PxForceMode::Enum>(forceMode));
    }

    void PhysicsActor::addTorque(glm::vec3 force, PhysicsForceMode forceMode) {
        if (!dynamic || kinematic) {
            return;
        }

        physxActor->is<physx::PxRigidDynamic>()->addTorque(PhysXUtils::glmToPhysXVec(force), static_cast<physx::PxForceMode::Enum>(forceMode));
    }

    void PhysicsActor::setMaxLinearVelocity(float velocity) {
        if (!dynamic) {
            return;
        }

        physxActor->is<physx::PxRigidDynamic>()->setMaxLinearVelocity(velocity);
    }

    void PhysicsActor::setMaxAngularVelocity(float velocity) {
        if (!dynamic) {
            return;
        }

        physxActor->is<physx::PxRigidDynamic>()->setMaxAngularVelocity(velocity);
    }

    uint32_t PhysicsActor::addBoxCollider(PhysicsMaterial& material, glm::vec3 halfSize, glm::vec3 offset, bool isTrigger) {
        auto* collider = new PhysicsShapeBox(*this, material, halfSize, offset, isTrigger);
        uint32_t uuid = Uuid().getUuid();
        colliders.insert({uuid, collider});
        return uuid;
    }

    uint32_t PhysicsActor::addSphereCollider(PhysicsMaterial& material, float radius, glm::vec3 offset, bool isTrigger) {
        auto* collider = new PhysicsShapeSphere(*this, material, radius, offset, isTrigger);
        uint32_t uuid = Uuid().getUuid();
        colliders.insert({uuid, collider});
        return uuid;
    }

    void PhysicsActor::removeCollider(uint32_t colliderUuid) {
        if (colliders.count(colliderUuid) != 0) {
            auto* collider = colliders[colliderUuid];
            collider->detachFromActor(physxActor);
            delete collider;
            colliders.erase(colliderUuid);
        }
    }
}