#include "StartScript.h"
#include "Events/Input.h"
#include "Application.h"

namespace Game {
    void StartScript::onAttach() {
        CgEngine::Input::setCursorMode(CgEngine::CursorMode::Normal);
    }

    void StartScript::onMouseButtonPressed(CgEngine::MouseButtonPressedEvent& event) {
        auto& canvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("canvas"));

        if (canvas.getUIElement<CgEngine::UiRect>("playButton")->isPointInside({event.getXPos(), event.getYPos()})) {
            setActiveScene("scenes/game_scene.xml");
        }

        if (canvas.getUIElement<CgEngine::UiRect>("exitButton")->isPointInside({event.getXPos(), event.getYPos()})) {
            CgEngine::Application::get().shutdown();
        }

    }

    void StartScript::onMouseMoved(CgEngine::MouseMovedEvent& event) {
        auto& canvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("canvas"));

        auto* playButton = canvas.getUIElement<CgEngine::UiRect>("playButton");
        auto* playText = canvas.getUIElement<CgEngine::UiText>("playText");
        if (playButton->isPointInside({event.getXPos(), event.getYPos()})) {
            playText->setColor({0.8f, 0.0f, 0.0f, 1.0f});
            playButton->setLineWidth(5);
        } else {
            playText->setColor({0.0f, 0.0f, 0.0f, 1.0f});
            playButton->setLineWidth(0);
        }

        auto* controlsButton = canvas.getUIElement<CgEngine::UiRect>("controlsButton");
        auto* controlsText = canvas.getUIElement<CgEngine::UiText>("controlsText");
        if (controlsButton->isPointInside({event.getXPos(), event.getYPos()})) {
            controlsText->setColor({0.8f, 0.0f, 0.0f, 1.0f});
            controlsButton->setLineWidth(5);
        } else {
            controlsText->setColor({0.0f, 0.0f, 0.0f, 1.0f});
            controlsButton->setLineWidth(0);
        }

        auto* exitButton = canvas.getUIElement<CgEngine::UiRect>("exitButton");
        auto* exitText = canvas.getUIElement<CgEngine::UiText>("exitText");
        if (exitButton->isPointInside({event.getXPos(), event.getYPos()})) {
            exitText->setColor({0.8f, 0.0f, 0.0f, 1.0f});
            exitButton->setLineWidth(5);
        } else {
            exitText->setColor({0.0f, 0.0f, 0.0f, 1.0f});
            exitButton->setLineWidth(0);
        }
    }
}