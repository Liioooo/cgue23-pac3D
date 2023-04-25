#include "PlayerScript.h"
#include "Events/Input.h"
#include "Events/KeyCodes.h"
#include "glm/gtx/common.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "Application.h"
#include "GhostsController.h"

namespace Game {
    void PlayerScript::onAttach() {
        CgEngine::Input::setCursorMode(CgEngine::CursorMode::Locked);
        prevMousePos = CgEngine::Input::getMousePosition();

        cameraRaycastExcluded.insert(getOwingEntity());
    }

    void PlayerScript::fixedUpdate(CgEngine::TimeStep ts) {
        if (respawnTimer > 0.0f) {
            respawnTimer -= ts.getSeconds();
            CG_LOGGING_INFO("Respawn in: {0} Seconds", respawnTimer);
            if (respawnTimer < 0.0f) {
                CG_LOGGING_INFO("Remaining lives: {0}", playerLives);
            }
            return;
        }

        glm::vec3 cameraDirection = glm::normalize(glm::quat({0.0f, yaw, 0.0f}) * glm::vec3(0, 0, -1));
        auto movementDirection = glm::vec3(0.0f);

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::W)) {
            movementDirection += cameraDirection;
        }
        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::A)) {
            movementDirection += glm::rotateY(cameraDirection, glm::radians(90.0f));
        }
        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::D)) {
            movementDirection += glm::rotateY(cameraDirection, glm::radians(-90.0f));
        }

        auto& comp = getComponent<CgEngine::CharacterControllerComponent>();

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::S)) {
            comp.move(-cameraDirection * ts.getSeconds() * 7.0f);
        } else if (movementDirection.x != 0 || movementDirection.y != 0 || movementDirection.z != 0) {
            comp.move(glm::normalize(movementDirection) * ts.getSeconds() * 7.0f);
        }
    }


    void PlayerScript::update(CgEngine::TimeStep ts) {}

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
        if (getEntityTag(other) == "coin") {
            CgEngine::Entity coinContainer = getParentEntity(other);
            auto& ghostsController = getComponent<CgEngine::ScriptComponent>(getParentEntity(coinContainer)).getNativeScript<GhostsController>();
            destroyEntity(other);
            CG_LOGGING_INFO("Left Coins: {0}/{1}", getChildEntities(coinContainer).size(), ghostsController.getTotalCoinAmount());
        }
    }

    void PlayerScript::onCollisionEnter(CgEngine::Entity other) {
        if (getEntityTag(other) == "ghost") {
            getComponent<CgEngine::CharacterControllerComponent>().setPosition({0.0f, 2.0f, 10.0f});
            pitch = -0.6f;
            yaw = 0.0f;
            respawnTimer = 3.0f;
            playerLives--;
            if (playerLives == 0) {
                CG_LOGGING_INFO("You lose!")
                CgEngine::Application::get().shutdown();
            }
        }
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
