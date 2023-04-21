#include "PlayerScript.h"
#include "Events/Input.h"
#include "Events/KeyCodes.h"
#include "glm/gtx/common.hpp"

namespace Game {
    void PlayerScript::onAttach() {
        CgEngine::Input::setCursorMode(CgEngine::CursorMode::Locked);
        prevMousePos = CgEngine::Input::getMousePosition();
    }

    void PlayerScript::fixedUpdate() {
        glm::vec3 movementDirection = glm::normalize(glm::quat({0.0f, yaw, 0.0f}) * glm::vec3(0, 0, -1));

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::W)) {
            auto& comp = getComponent<CgEngine::CharacterControllerComponent>();
            comp.move(movementDirection * 0.05f);
        }
    }


    void PlayerScript::update(CgEngine::TimeStep ts) {
    }

    void PlayerScript::lateUpdate(CgEngine::TimeStep ts) {
        auto mousePos = CgEngine::Input::getMousePosition();

        float mouseDeltaX = (prevMousePos.first - mousePos.first) * 0.001f;
        float mouseDeltaY = (prevMousePos.second - mousePos.second) * 0.0015f;

        prevMousePos = mousePos;

        glm::vec3 cameraCenter = getComponent<CgEngine::TransformComponent>().getGlobalPosition();

        CgEngine::Entity cameraEntity = *getChildEntities().begin();
        auto& cameraTransform = getComponent<CgEngine::TransformComponent>(cameraEntity);

        pitch = glm::clamp(pitch + mouseDeltaY, glm::radians(-80.0f), glm::radians(0.0f));
        yaw = glm::fmod(yaw + mouseDeltaX, glm::two_pi<float>());

        glm::vec3 cameraDirection = glm::normalize(glm::quat({pitch, yaw, 0}) * glm::vec3(0, 0, -1));


        cameraTransform.setYawPitchRoll(yaw, pitch, 0);
        cameraTransform.setLocalPosition(cameraDirection * -cameraDistance);
    }

    void PlayerScript::onMouseScrolled(CgEngine::MouseScrolledEvent& event) {
        if (event.getYOffsetPos() > 0) {
            cameraDistance -= 0.2f;
        } else {
            cameraDistance += 0.2f;
        }
        cameraDistance = glm::clamp(cameraDistance, 2.0f, 6.0f);
    }
}
