#pragma once

#include "Scene/Entity.h"
#include "PxPhysicsAPI.h"
#include "AbstractPhysicsActor.h"

namespace CgEngine {

    class Scene;

    class PhysicsController : public AbstractPhysicsActor {
    public:
        PhysicsController(physx::PxController* physXController, bool hasGravity, glm::vec3 gravity, Entity entity, Scene& scene);
        ~PhysicsController();

        PhysicsActorType getPhysicsActorType() const override;

        void update(float ts);
        void updateTransforms();

        void move(glm::vec3 dir);
        void setPosition(glm::vec3 pos);
        void jump(float strength);

        bool standsOnGround() const;

    private:
        physx::PxController* physXController;
        bool hasGravity;
        Entity entity;

        glm::vec3 currentMovement = glm::vec3(0.0f);
        float currentJumpSpeed = 0;
        glm::vec3 gravity;
        physx::PxControllerCollisionFlags collisionFlags;

        glm::vec3 desiredPosition;
        bool shouldTeleport = false;
    };

}
