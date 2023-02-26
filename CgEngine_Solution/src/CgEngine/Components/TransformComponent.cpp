#include "TransformComponent.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Logging.h"

namespace CgEngine {
    void TransformComponent::onAttach(Scene& scene, TransformComponentParams &params) {
        localPosition = params.position;
        localRotation = params.rotation;
        localScale = params.scale;
    }

    const glm::vec3& TransformComponent::getLocalPosition() const {
        return localPosition;
    }

    const glm::vec3& TransformComponent::getLocalRotation() const {
        return localRotation;
    }

    const glm::vec3& TransformComponent::getLocalScale() const {
        return localScale;
    }

    const glm::vec3& TransformComponent::getGlobalPosition() const {
        return globalPosition;
    }

    const glm::vec3& TransformComponent::getGlobalRotation() const {
        return globalRotation;
    }

    const glm::vec3& TransformComponent::getGlobalScale() const {
        return globalScale;
    }

    const glm::mat4& TransformComponent::getModelMatrix() const {
        return modelMatrix;
    }

    void TransformComponent::setLocalPosition(glm::vec3 position) {
        isDirty = true;
        localPosition = position;
    }

    void TransformComponent::setLocalRotation(glm::vec3 rotation) {
        isDirty = true;
        localRotation = rotation;
    }

    void TransformComponent::setLocalScale(glm::vec3 scale) {
        isDirty = true;
        localScale = scale;
    }

    void TransformComponent::setYawPitchRoll(float yaw, float pitch, float roll) {
        isDirty = true;
        yawPitchRoll = {yaw, pitch, roll};
    }

    bool TransformComponent::_calculateTopLevelTransforms() {
        if (isDirty) {
            globalPosition = localPosition;
            globalRotation = localRotation;
            globalScale = localScale;

            if (yawPitchRoll.x != 0 || yawPitchRoll.y != 0 || yawPitchRoll.z != 0) {
                glm::vec3 direction = glm::normalize(glm::quat({yawPitchRoll.y, yawPitchRoll.x, yawPitchRoll.z}) * glm::vec3(0, 0, -1));
                localModelMatrix = glm::inverse(glm::lookAt(localPosition, localPosition + direction, {0.0f, 1.0f, 0.0f}));
            } else {
                localModelMatrix = calculateModelMatrix(localPosition, localRotation, localScale);
            }
            modelMatrix = localModelMatrix;
            isDirty = false;
            physicsDirty = false;
            return true;
        } else if (physicsDirty) {
            modelMatrix = calculateModelMatrix(globalPosition, globalRotation, globalScale);
            physicsDirty = false;
            return true;
        }
        return false;
    }

    void TransformComponent::_physicsUpdate() {
        // TODO: Set GlobalProperties

        physicsDirty = true;
    }

    bool TransformComponent::_calculateChildTransformsWithParent(const glm::mat4& parentModelMatrix, bool parentDirty) {
        if (isDirty || parentDirty) {
            if (isDirty) {
                if (yawPitchRoll.x != 0 || yawPitchRoll.y != 0 || yawPitchRoll.z != 0) {
                    glm::vec3 direction = glm::normalize(glm::quat({yawPitchRoll.y, yawPitchRoll.x, yawPitchRoll.z}) * glm::vec3(0, 0, -1));
                    localModelMatrix = glm::inverse(glm::lookAt(localPosition, localPosition + direction, {0.0f, 1.0f, 0.0f}));
                } else {
                    localModelMatrix = calculateModelMatrix(localPosition, localRotation, localScale);
                }
            }
            modelMatrix = parentModelMatrix * localModelMatrix;
            // TODO: Decompose to globals
            isDirty = false;
            physicsDirty = false;
            return true;
        } else if (physicsDirty) {
            modelMatrix = calculateModelMatrix(globalPosition, globalRotation, globalScale);
            physicsDirty = false;
            return true;
        }
        return false;
    }

    glm::mat4 TransformComponent::calculateModelMatrix(glm::vec3& pos, glm::vec3& rot, glm::vec3& scale) {
        auto m = glm::mat4(1.0f);
        m = glm::translate(m, pos);
        m = glm::rotate(m, rot.x, glm::vec3(1, 0, 0));
        m = glm::rotate(m, rot.y, glm::vec3(0, 1, 0));
        m = glm::rotate(m, rot.z, glm::vec3(0, 0, 1));
        m = glm::scale(m, scale);
        return m;
    }
}
