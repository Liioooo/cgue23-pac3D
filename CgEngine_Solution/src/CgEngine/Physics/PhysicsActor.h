#pragma once

#include "PxPhysicsAPI.h"
#include "Physics/PhysicsSystem.h"
#include "Scene/Entity.h"
#include "PhysicsShape.h"

namespace CgEngine {

    class Scene;

    class PhysicsActor {
    public:
        PhysicsActor(Scene* scene, Entity entity, glm::vec3 pos, glm::quat orientation, bool isDynamic, PhysicsCollisionDetection collisionDetection);
        ~PhysicsActor();

        physx::PxRigidActor& getPhysxActor() const;
        const physx::PxFilterData& getFilterData() const;
        Scene& getScene() const;
        Entity getEntity() const;

        bool isSleeping();

        void updateTransforms();

        void setKinematic(bool isKinematic);
        void setMass(float mass);
        void setAngularDrag(float drag);
        void setLinearDrag(float drag);
        void setGravityDisabled(bool disabled);

        bool isDynamic() const;
        bool isKinematic() const;

        void addForce(glm::vec3 force, PhysicsForceMode forceMode);
        void addTorque(glm::vec3 force, PhysicsForceMode forceMode);
        void setKinematicTarget(glm::vec3 target, glm::quat rotation);

        void setMaxLinearVelocity(float velocity);
        void setMaxAngularVelocity(float velocity);

        uint32_t addBoxCollider(PhysicsMaterial& material, glm::vec3 halfSize, glm::vec3 offset, bool isTrigger);
        uint32_t addSphereCollider(PhysicsMaterial& material, float radius, glm::vec3 offset, bool isTrigger);
        uint32_t addCapsuleCollider(PhysicsMaterial& material, float radius, float halfHeight, glm::vec3 offset, bool isTrigger);
        void removeCollider(uint32_t colliderUuid);

    private:
        physx::PxRigidActor* physxActor;
        physx::PxFilterData filterData;
        bool dynamic;
        bool kinematic = false;
        Scene* scene;
        Entity entity;

        std::unordered_map<uint32_t, PhysicsShape*> colliders;
    };

}
