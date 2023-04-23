#pragma once

#include "Scene/Entity.h"
#include "PxPhysicsAPI.h"

namespace CgEngine {

    class Scene;

    class PhysicsController {
    public:
        PhysicsController(physx::PxController* physXController, bool hasGravity, Entity entity);
        ~PhysicsController();

        void update(float ts);
        void updateTransforms(Scene& scene);

        void move(glm::vec3 dir);
        void jump(float strength);

        bool standsOnGround();

    private:
        physx::PxController* physXController;
        bool hasGravity;
        Entity entity;

        glm::vec3 currentMovement = glm::vec3(0.0f);
        float currentJumpSpeed = 0;
        glm::vec3 gravity;
        physx::PxControllerCollisionFlags collisionFlags;
    };

}
