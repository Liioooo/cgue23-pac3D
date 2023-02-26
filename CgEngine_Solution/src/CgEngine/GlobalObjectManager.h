#pragma once

#include <Scripting/ScriptManager.h>
#include "Scene/SceneManager.h"
#include "Resources/ResourceManager.h"


namespace CgEngine {

    class SceneManager;

    class GlobalObjectManager {
    public:
        static GlobalObjectManager& getInstance();

        ~GlobalObjectManager();

        SceneManager& getSceneManager() const;
        ResourceManager& getResourceManager() const;
        ScriptManager& getScriptManager() const;

    private:
        GlobalObjectManager();

        SceneManager* sceneManager;
        ResourceManager* resourceManager;
        ScriptManager* scriptManager;
    };
}



