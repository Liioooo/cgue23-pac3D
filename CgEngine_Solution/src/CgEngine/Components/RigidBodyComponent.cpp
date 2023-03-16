#include "RigidBodyComponent.h"
#include "Scene/Scene.h"

namespace CgEngine {
    void RigidBodyComponent::onAttach(Scene& scene, RigidBodyComponentParams& params) {
        auto& transform = scene.getComponent<TransformComponent>(entity);

        actor = new PhysicsActor(&scene, entity, transform.getGlobalPosition(), transform.getGlobalRotationQuat(), params.isDynamic, params.collisionDetection);

        if (params.isDynamic) {
            setKinematic(params.isKinematic);
            setGravityDisabled(params.disableGravity);
            setMass(params.mass);
            setLinearDrag(params.linearDrag);
            setAngularDrag(params.angularDrag);
        }

        if (scene.hasComponent<BoxColliderComponent>(entity)) {
            auto& colliderComp = scene.getComponent<BoxColliderComponent>(entity);
            uint32_t colliderUuid = addBoxCollider(colliderComp.getPhysicsMaterial(), colliderComp.getHalfSize(), colliderComp.getOffset(), colliderComp.getIsTrigger());
            colliderComp.colliderUuid = colliderUuid;
        }

        scene.getPhysicsScene().addActor(actor);
    }

    void RigidBodyComponent::onDetach(Scene& scene) {
        scene.getPhysicsScene().removeActor(actor);
        delete actor;
    }

    void RigidBodyComponent::setKinematic(bool isKinematic) {
        actor->setKinematic(isKinematic);
    }

    void RigidBodyComponent::setMass(float mass) {
        actor->setMass(mass);
    }

    void RigidBodyComponent::setLinearDrag(float drag) {
        actor->setLinearDrag(drag);
    }

    void RigidBodyComponent::setAngularDrag(float drag) {
        actor->setAngularDrag(drag);
    }

    void RigidBodyComponent::setGravityDisabled(bool disabled) {
        actor->setGravityDisabled(disabled);
    }

    bool RigidBodyComponent::isDynamic() const {
        return actor->isDynamic();
    }

    bool RigidBodyComponent::isKinematic() const {
        return actor->isKinematic();
    }

    void RigidBodyComponent::addForce(glm::vec3 force, PhysicsForceMode forceMode) {
        actor->addForce(force, forceMode);
    }

    void RigidBodyComponent::addTorque(glm::vec3 force, PhysicsForceMode forceMode) {
        actor->addTorque(force, forceMode);
    }

    void RigidBodyComponent::setMaxLinearVelocity(float velocity) {
        actor->setMaxLinearVelocity(velocity);
    }

    void RigidBodyComponent::setMaxAngularVelocity(float velocity) {
        actor->setMaxAngularVelocity(velocity);
    }

    uint32_t RigidBodyComponent::addBoxCollider(PhysicsMaterial& material, glm::vec3 halfSize, glm::vec3 offset, bool isTrigger) {
        return actor->addBoxCollider(material, halfSize, offset, isTrigger);
    }

    void RigidBodyComponent::removeCollider(uint32_t colliderUuid) {
        actor->removeCollider(colliderUuid);
    }
}
