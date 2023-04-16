#include "Camera.h"
#include "glm/ext/matrix_clip_space.hpp"

namespace CgEngine {
    CameraProjectionType Camera::getProjectionType() const {
        return projectionType;
    }

    void Camera::setProjectionType(CameraProjectionType projectionType) {
        this->projectionType = projectionType;
        calculateProjection();
    }

    float Camera::getPerspectiveFov() const {
        return glm::degrees(perspectiveFov);
    }

    void Camera::setPerspectiveFov(float perspectiveFov) {
        this->perspectiveFov = glm::radians(perspectiveFov);
        calculateProjection();
    }

    float Camera::getPerspectiveNear() const {
        return perspectiveNear;
    }

    void Camera::setPerspectiveNear(float perspectiveNear) {
        this->perspectiveNear = perspectiveNear;
        calculateProjection();
    }

    float Camera::getPerspectiveFar() const {
        return perspectiveFar;
    }

    void Camera::setPerspectiveFar(float perspectiveFar) {
        this->perspectiveFar = perspectiveFar;
        calculateProjection();
    }

    float Camera::getOrthographicSize() const {
        return orthographicSize;
    }

    void Camera::setOrthographicSize(float orthographicSize) {
        this->orthographicSize = orthographicSize;
        calculateProjection();
    }

    float Camera::getOrthographicNear() const {
        return orthographicNear;
    }

    void Camera::setOrthographicNear(float orthographicNear) {
        this->orthographicNear = orthographicNear;
        calculateProjection();
    }

    float Camera::getOrthographicFar() const {
        return orthographicFar;
    }

    void Camera::setOrthographicFar(float orthographicFar) {
        Camera::orthographicFar = orthographicFar;
        calculateProjection();
    }

    float Camera::getAspectRatio() const {
        return aspectRatio;
    }

    const glm::mat4& Camera::getProjectionMatrix() const {
        return projectionMatrix;
    }

    void Camera::setViewportSize(int width, int height) {
        if (width != 0 && height != 0) {
            aspectRatio = (float)width / (float)height;
            calculateProjection();
        }
    }

    void Camera::calculateProjection() {
        if (projectionType == CameraProjectionType::Perspective) {
            projectionMatrix = glm::perspective(perspectiveFov, aspectRatio, perspectiveNear, perspectiveFar);
        } else {
            float orthoLeft = -orthographicSize * aspectRatio * 0.5f;
            float orthoRight = orthographicSize * aspectRatio * 0.5f;
            float orthoBottom = -orthographicSize * 0.5f;
            float orthoTop = orthographicSize * 0.5f;
            projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, orthographicNear, orthographicFar);
        }
    }
}
