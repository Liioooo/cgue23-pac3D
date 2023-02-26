#pragma once

#include "GlobalObjectManager.h"

namespace CgEngine {

    GlobalObjectManager &GlobalObjectManager::getInstance() {
        static GlobalObjectManager instance;
        return instance;
    }

    CgEngine::GlobalObjectManager::GlobalObjectManager() {
        sceneManager = new SceneManager();
        resourceManager = new ResourceManager();
        scriptManager = new ScriptManager();
    }

    GlobalObjectManager::~GlobalObjectManager() {
        delete sceneManager;
        delete resourceManager;
    }

    SceneManager& GlobalObjectManager::getSceneManager() const {
        return *sceneManager;
    }

    ResourceManager &GlobalObjectManager::getResourceManager() const {
        return *resourceManager;
    }

    ScriptManager &GlobalObjectManager::getScriptManager() const {
        return *scriptManager;
    }

}



