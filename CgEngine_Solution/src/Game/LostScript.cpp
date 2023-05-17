#include "LostScript.h"
#include "Events/Input.h"
#include "Application.h"

namespace Game {
    void LostScript::onAttach() {
        CgEngine::Input::setCursorMode(CgEngine::CursorMode::Normal);
    }

    void LostScript::onMouseButtonPressed(CgEngine::MouseButtonPressedEvent& event) {
        auto& canvas = getComponent<CgEngine::UiCanvasComponent>(findEntityById("canvas"));

        if (canvas.getUIElement<CgEngine::UiRect>("againButton")->isPointInside({event.getXPos(), event.getYPos()})) {
            setActiveScene("scenes/default_scene.xml");
        }

        if (canvas.getUIElement<CgEngine::UiRect>("exitButton")->isPointInside({event.getXPos(), event.getYPos()})) {
            CgEngine::Application::get().shutdown();
        }

    }
}