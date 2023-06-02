#include "PlayerScript.h"
#include "Events/Input.h"
#include "Events/KeyCodes.h"
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
        auto& playerMeshTransform = getComponent<CgEngine::TransformComponent>(findEntityById("playerMesh"));
        playerMeshTransform.setLocalRotationVec({0.0f, yaw, 0.0f});

        if (respawnTimer > 0.0f) {
            respawnTimer -= ts.getSeconds();

            auto& gameCanvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("inGameCanvas"));
            if (respawnTimer > 0.0f) {
                std::string timerStr = std::to_string(respawnTimer);
                gameCanvas.getUIElement<CgEngine::UiText>("startTimer")->setText(timerStr.substr(0, timerStr.find('.') + 3));
                getComponent<CgEngine::AnimatedMeshRendererComponent>(findEntityById("playerMesh")).setAnimationPlaying(false);
            } else {
                gameCanvas.removeUIElement("startTimer");
                gameCanvas.removeUIElement("start");
                gameCanvas.getUIElement<CgEngine::UiCircle>("cross")->setFillColor({0.0f, 0.0f, 0.0f, 0.8f});
                getComponent<CgEngine::AnimatedMeshRendererComponent>(findEntityById("playerMesh")).reset();
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

        getComponent<CgEngine::AnimatedMeshRendererComponent>(findEntityById("playerMesh")).setAnimationPlaying(glm::length(movement) > 0.0f);

        comp.move(movement);
    }


    void PlayerScript::update(CgEngine::TimeStep ts) {
        timeSinceNewProjectile += ts.getSeconds();
        if (timeSinceNewProjectile >= 5.0f) {
            timeSinceNewProjectile = 0.0f;
            if (projectiles < maxProjectiles) {
                projectiles++;
            }
        }
        updateGuiStats();
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
        if (getEntityTag(other) == "coin") {
            CgEngine::Entity coinContainer = getParentEntity(other);
            auto& ghostsController = getComponent<CgEngine::ScriptComponent>(getParentEntity(coinContainer)).getNativeScript<GhostsController>();
            destroyEntity(other);
            totalCoins = ghostsController.getTotalCoinAmount();
            leftCoins = getChildEntities(coinContainer).size();
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
                setActiveScene("scenes/lost_scene.xml");
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

    void PlayerScript::onMouseButtonPressed(CgEngine::MouseButtonPressedEvent& event) {
        if (respawnTimer > 0 || projectiles <= 0) {
            return;
        }

        projectiles--;

        auto& playerTransform = getComponent<CgEngine::TransformComponent>();

        CgEngine::Entity e = createEntity(findEntityById("projectiles"));
        setEntityTag(e, "projectile");

        CgEngine::RigidBodyComponentParams rigidBodyParams{};
        rigidBodyParams.isDynamic = true;
        rigidBodyParams.mass = 5;
        rigidBodyParams.angularDrag = 0.5f;
        rigidBodyParams.linearDrag = 0.25f;
        rigidBodyParams.collisionDetection = CgEngine::PhysicsCollisionDetection::Continuous;

        attachComponent<CgEngine::TransformComponent>(e, CgEngine::TransformComponentParams{playerTransform.getGlobalPosition() + glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.15f)});
        attachComponent<CgEngine::MeshRendererComponent>(e, CgEngine::MeshRendererComponentParams{"", "CG_SphereMesh", "Projectiles", true});
        attachComponent<CgEngine::PointLightComponent>(e, CgEngine::PointLightComponentParams{{10.0f/255, 23.0f/255, 87.0f/255}, 1.5f, 4.0f, 0.8f});
        attachComponent<CgEngine::SphereColliderComponent>(e, CgEngine::SphereColliderComponentParams{1.0f, glm::vec3(0.0f), false, "projectiles"});
        attachComponent<CgEngine::ScriptComponent>(e, CgEngine::ScriptComponentParams{"projectileScript"});
        auto& rigidBody = attachComponent<CgEngine::RigidBodyComponent>(e, rigidBodyParams);

        glm::vec3 direction = glm::normalize(glm::quat({glm::max(0.0f, pitch + 0.25f), yaw, 0}) * glm::vec3(0, 0, -1));

        rigidBody.addForce(direction * 8000.0f);

        event.stopPropagation();
    }

    void PlayerScript::createStartText() {
        auto& gameCanvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("inGameCanvas"));
        auto* start = gameCanvas.addUiText("start");
        auto* startTimer = gameCanvas.addUiText("startTimer");

        start->setFont("SpaceMono-Bold.ttf");
        start->setText("START IN");
        start->setTop(0.3f, CgEngine::UIPosUnit::VHPercent);
        start->setLeft(0.5f, CgEngine::UIPosUnit::VWPercent);
        start->setSize(0.12f, CgEngine::UIPosUnit::VHPercent);
        start->setXAlignment(CgEngine::UIXAlignment::Center);
        start->setYAlignment(CgEngine::UIYAlignment::Center);
        start->setColor({0.8f, 0.0f, 0.0f, 1.0f});

        std::string timerStr = std::to_string(respawnTimer);

        startTimer->setFont("SpaceMono-Bold.ttf");
        startTimer->setText(timerStr.substr(0, timerStr.find('.') + 3));
        startTimer->setTop(0.44f, CgEngine::UIPosUnit::VHPercent);
        startTimer->setLeft(0.5f, CgEngine::UIPosUnit::VWPercent);
        startTimer->setSize(0.18f, CgEngine::UIPosUnit::VHPercent);
        startTimer->setXAlignment(CgEngine::UIXAlignment::Center);
        startTimer->setYAlignment(CgEngine::UIYAlignment::Center);
        startTimer->setColor({0.0f, 0.0f, 0.0f, 1.0f});
        startTimer->setUseKerning(false);

        gameCanvas.getUIElement<CgEngine::UiCircle>("cross")->setFillColor({0.0f, 0.0f, 0.0f, 0.0f});
    }

    void PlayerScript::updateGuiStats() {
        auto& gameCanvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("inGameCanvas"));
        auto* projectilesAmount = gameCanvas.getUIElement<CgEngine::UiRect>("projectilesAmount");

        float projectileWidth = (static_cast<float>(projectiles) / static_cast<float>(maxProjectiles)) * 0.15f;
        projectilesAmount->setWidth(projectileWidth, CgEngine::UIPosUnit::VWPercent);

        auto* coinsAmount = gameCanvas.getUIElement<CgEngine::UiRect>("coinsAmount");
        float coinWidth = (1.0f - (static_cast<float>(leftCoins) / static_cast<float>(totalCoins))) * 0.15f;
        coinsAmount->setWidth(coinWidth, CgEngine::UIPosUnit::VWPercent);

    }
}
