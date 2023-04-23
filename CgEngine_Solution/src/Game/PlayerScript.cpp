#include "PlayerScript.h"
#include "Events/Input.h"
#include "Events/KeyCodes.h"
#include "glm/gtx/common.hpp"
#include "glm/gtx/rotate_vector.hpp"

namespace Game {
    void PlayerScript::onAttach() {
        CgEngine::Input::setCursorMode(CgEngine::CursorMode::Locked);
        prevMousePos = CgEngine::Input::getMousePosition();

        cameraRaycastExcluded.insert(getOwingEntity());
    }

    void PlayerScript::fixedUpdate() {
        glm::vec3 cameraDirection = glm::normalize(glm::quat({0.0f, yaw, 0.0f}) * glm::vec3(0, 0, -1));
        auto forwardDirection = cameraDirection;

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::A)) {
            forwardDirection = glm::rotateY(cameraDirection, glm::radians(40.0f));
        }
        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::D)) {
            forwardDirection = glm::rotateY(cameraDirection, glm::radians(-40.0f));
        }

        auto& comp = getComponent<CgEngine::CharacterControllerComponent>();

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::W)) {
            comp.move(forwardDirection * 0.1f);
        } else if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::S)) {
            comp.move(-cameraDirection * 0.1f);
        }
    }


    void PlayerScript::update(CgEngine::TimeStep ts) {
    }

    void PlayerScript::lateUpdate(CgEngine::TimeStep ts) {
        auto mousePos = CgEngine::Input::getMousePosition();

        float mouseDeltaX = (prevMousePos.first - mousePos.first) * 0.001f;
        float mouseDeltaY = (prevMousePos.second - mousePos.second) * 0.0015f;

        prevMousePos = mousePos;

        CgEngine::Entity cameraEntity = *getChildEntities().begin();
        auto& cameraTransform = getComponent<CgEngine::TransformComponent>(cameraEntity);

        pitch = glm::clamp(pitch + mouseDeltaY, glm::radians(-80.0f), glm::radians(10.0f));
        yaw = glm::fmod(yaw + mouseDeltaX, glm::two_pi<float>());

        glm::vec3 cameraDirection;

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::V)) {
            cameraDirection = glm::normalize(glm::quat({pitch, yaw + glm::pi<float>(), 0}) * glm::vec3(0, 0, -1));
            cameraTransform.setYawPitchRoll(yaw + glm::pi<float>(), pitch, 0);

        } else {
            cameraDirection = glm::normalize(glm::quat({pitch, yaw, 0}) * glm::vec3(0, 0, -1));
            cameraTransform.setYawPitchRoll(yaw, pitch, 0);
        }

        glm::vec3 cameraCenter = getComponent<CgEngine::TransformComponent>().getGlobalPosition() + cameraOffset;

        auto raycastHit = physicsRaycast(cameraCenter, -cameraDirection, cameraDistance, cameraRaycastExcluded);
        if (raycastHit.hitFound) {
            cameraTransform.setLocalPosition((cameraDirection * -(raycastHit.hitDistance - 0.1f)) + cameraOffset);
        } else {
            cameraTransform.setLocalPosition((cameraDirection * -cameraDistance) + cameraOffset);
        }
    }

    void PlayerScript::onTriggerEnter(CgEngine::Entity other) {
        destroyEntity(other);
    }

    void PlayerScript::onMouseScrolled(CgEngine::MouseScrolledEvent& event) {
        if (event.getYOffsetPos() > 0) {
            cameraDistance -= 0.4f;
        } else {
            cameraDistance += 0.4f;
        }
        cameraDistance = glm::clamp(cameraDistance, 3.0f, 6.0f);
    }

    void PlayerScript::onKeyPressed(CgEngine::KeyPressedEvent& event) {
        if (event.getKeyCode() == CgEngine::KeyCode::Space) {
            auto& comp = getComponent<CgEngine::CharacterControllerComponent>();
            if (comp.standsOnGround()) {
                comp.jump(0.6);
            }
        }
    }
}
