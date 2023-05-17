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

        createStartText();
    }

    void PlayerScript::fixedUpdate(CgEngine::TimeStep ts) {
        if (respawnTimer > 0.0f) {
            respawnTimer -= ts.getSeconds();

            auto& gameCanvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("inGameCanvas"));
            if (respawnTimer > 0) {
                std::string timerStr = std::to_string(respawnTimer);
                gameCanvas.getUIElement<CgEngine::UiText>("startTimer")->setText(timerStr.substr(0, timerStr.find('.') + 3));
            } else {
                gameCanvas.removeUIElement("startTimer");
                gameCanvas.removeUIElement("start");
            }

            return;
        }

        glm::vec3 cameraDirection = glm::normalize(glm::quat({0.0f, yaw, 0.0f}) * glm::vec3(0, 0, -1));
        auto movement = glm::vec3(0.0f);

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::W)) {
            movement += cameraDirection;
        }
        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::A)) {
            movement += glm::rotateY(cameraDirection, glm::radians(90.0f));
        }
        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::D)) {
            movement += glm::rotateY(cameraDirection, glm::radians(-90.0f));
        }

        auto& comp = getComponent<CgEngine::CharacterControllerComponent>();

        if (CgEngine::Input::isKeyPressed(CgEngine::KeyCode::S)) {
            movement = -cameraDirection * ts.getSeconds() * 7.0f;
        } else {
            movement = (glm::length(movement) == 0.0f ? movement : normalize(movement)) * ts.getSeconds() * 7.0f;
        }

        comp.move(movement);
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
        if (getEntityTag(other) == "ghost" && respawnTimer <= 0.0f) {
            getComponent<CgEngine::CharacterControllerComponent>().setPosition({0.0f, 2.0f, 10.0f});
            pitch = -0.6f;
            yaw = 0.0f;
            respawnTimer = 3.0f;
            playerLives--;

            auto& gameCanvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("inGameCanvas"));
            gameCanvas.removeUIElement("live" + std::to_string(playerLives));

            if (playerLives == 0) {
                CG_LOGGING_INFO("You lose!")
                CgEngine::Application::get().shutdown();
            }

            createStartText();
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
            if (comp.isGrounded() && respawnTimer <= 0.0f) {
                comp.jump(0.5f);
            }
        }
    }

    void PlayerScript::createStartText() {
        auto& gameCanvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("inGameCanvas"));
        auto* start = gameCanvas.addUiText("start");
        auto* startTimer = gameCanvas.addUiText("startTimer");

        start->setFont("SpaceMono-Bold.ttf");
        start->setText("START IN");
        start->setTop(0.3, CgEngine::UIPosUnit::Percent);
        start->setLeft(0.5, CgEngine::UIPosUnit::Percent);
        start->setSize(120);
        start->setXAlignment(CgEngine::UIXAlignment::Center);
        start->setYAlignment(CgEngine::UIYAlignment::Center);
        start->setColor({0.8f, 0.0f, 0.0f, 1.0f});

        std::string timerStr = std::to_string(respawnTimer);

        startTimer->setFont("SpaceMono-Bold.ttf");
        startTimer->setText(timerStr.substr(0, timerStr.find('.') + 3));
        startTimer->setTop(0.42, CgEngine::UIPosUnit::Percent);
        startTimer->setLeft(0.5, CgEngine::UIPosUnit::Percent);
        startTimer->setSize(160);
        startTimer->setXAlignment(CgEngine::UIXAlignment::Center);
        startTimer->setYAlignment(CgEngine::UIYAlignment::Center);
        startTimer->setColor({0.0f, 0.0f, 0.0f, 1.0f});
        startTimer->setUseKerning(false);
    }
}
