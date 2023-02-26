#pragma once

#include "Scene.h"
#include "Resources/XMLFile.h"

namespace CgEngine {

    class SceneLoader {
    public:
        static Scene* loadScene(XMLFile* xmlSceneFile, int viewportWidth, int viewportHeight);

    private:
        static void createEntity(Scene* scene, Entity parent, const pugi::xml_node& node);
        static void createComponent(Scene* scene, Entity entity, const pugi::xml_node& node);
        static void createTransformComponent(Scene* scene, Entity entity, const pugi::xml_node& node);
        static void createMeshRendererComponent(Scene* scene, Entity entity, const pugi::xml_node& node);
        static void createCameraComponent(Scene* scene, Entity entity, const pugi::xml_node& node);
        static void createScriptComponent(Scene* scene, Entity entity, const pugi::xml_node& node);

        static glm::vec3 stringTupleToVec3(const std::string& s);

    };

}
