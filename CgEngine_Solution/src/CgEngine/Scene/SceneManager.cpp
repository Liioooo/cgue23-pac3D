#include "SceneManager.h"
#include "GlobalObjectManager.h"
#include "Resources/XMLFile.h"
#include "SceneLoader.h"

namespace CgEngine {

    Scene* SceneManager::getActiveScene() {
        if (nextScene != nullptr) {
            delete activeScene;
            activeScene = nextScene;
            nextScene = nullptr;
        }
        return activeScene;
    }

    void SceneManager::setActiveScene(const std::string &name) {
        auto* xmlSceneFile = GlobalObjectManager::getInstance().getResourceManager().getResource<XMLFile>(name);
        nextScene = SceneLoader::loadScene(xmlSceneFile, viewportWidth, viewportHeight);
        nextScene->updateTransforms();
    }

    void SceneManager::setViewportSize(uint32_t width, uint32_t height) {
        viewportWidth = width;
        viewportHeight = height;
        if (activeScene != nullptr) {
            activeScene->onViewportResize(width, height);
        }
        if (nextScene != nullptr) {
            nextScene->onViewportResize(width, height);
        }
    }
}
