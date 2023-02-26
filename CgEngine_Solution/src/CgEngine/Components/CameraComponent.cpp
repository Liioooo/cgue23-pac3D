#include "CameraComponent.h"
#include "Scene/Scene.h"

namespace CgEngine {
    void CameraComponent::onAttach(Scene& scene, CameraComponentParams &params) {
        camera = Camera();

        isPrimaryCam = params.isPrimary;
        camera.setViewportSize(scene.getViewportWidth(), scene.getViewportHeight());

        if (params.projection == "perspective") {
            camera.setProjectionType(CameraProjectionType::Perspective);
            camera.setPerspectiveFar(params.cfar);
            camera.setPerspectiveNear(params.cnear);
            camera.setPerspectiveFov(params.cfov);
        } else {
            camera.setProjectionType(CameraProjectionType::Orthographic);
            camera.setOrthographicFar(params.cfar);
            camera.setOrthographicNear(params.cnear);
            camera.setOrthographicSize(params.orthoSize);
        }
    }

    Camera& CameraComponent::getCamera() {
        return camera;
    }

    void CameraComponent::setPrimary(bool primary) {
        isPrimaryCam = primary;
    }

    bool CameraComponent::isPrimary() const {
        return isPrimaryCam;
    }
}
