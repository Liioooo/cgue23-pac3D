#include "SceneLoader.h"
#include "pugixml.hpp"

namespace CgEngine {
    Scene* SceneLoader::loadScene(XMLFile* xmlSceneFile, int viewportWidth, int viewportHeight) {
        const pugi::xml_document& xml = xmlSceneFile->getXMLDocument();
        auto* scene = new Scene(viewportWidth, viewportHeight);
        const auto& sceneNode = xml.child("Scene");
        for (const auto &item: sceneNode.children()) {
            createEntity(scene, CG_ENTITY_UNAVAILABLE, item);
        }

        return scene;
    }

    void SceneLoader::createEntity(Scene *scene, Entity parent, const pugi::xml_node& node) {
        const auto& idAttr = node.attribute("id");
        Entity entity;
        if (parent == CG_ENTITY_UNAVAILABLE) {
            entity = idAttr.empty() ? scene->createEntity() : scene->createEntity(idAttr.as_string());
        } else {
            entity = idAttr.empty() ? scene->createEntity(parent) : scene->createEntity(parent, idAttr.as_string());
        }

        const auto& componentsNode = node.child("Components");
        for (const auto &compNode: componentsNode.children()) {
            createComponent(scene, entity, compNode);
        }
        CG_ASSERT(scene->hasComponent<TransformComponent>(entity), "Every entity must contain a TransformComponent!");

        for (const auto &child: node.children("Entity")) {
            createEntity(scene, entity, child);
        }
    }

    void SceneLoader::createComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        std::string name = node.name();
        if (name == "TransformComponent") {
            createTransformComponent(scene, entity, node);
        } else if (name == "MeshRendererComponent") {
            createMeshRendererComponent(scene, entity, node);
        } else if (name == "CameraComponent") {
            createCameraComponent(scene, entity, node);
        } else if (name == "ScriptComponent") {
            createScriptComponent(scene, entity, node);
        }
    }

     void SceneLoader::createTransformComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        glm::vec3 position = stringTupleToVec3(node.attribute("position").as_string());
        glm::vec3 rotation = stringTupleToVec3(node.attribute("rotation").as_string());
        glm::vec3 scale = stringTupleToVec3(node.attribute("scale").as_string());
        TransformComponentParams params{position, rotation, scale};
        scene->attachComponent<TransformComponent>(entity, params);
    }

    void SceneLoader::createMeshRendererComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        MeshRendererComponentParams params{
            node.attribute("mesh").as_string(),
            node.attribute("material").as_string()
        };
        scene->attachComponent<MeshRendererComponent>(entity, params);
    }

    void SceneLoader::createCameraComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        CameraComponentParams params{
            node.attribute("projection").as_string(),
            node.attribute("near").as_float(0.1f),
            node.attribute("far").as_float(100.0f),
            node.attribute("fov").as_float(60.0f),
            node.attribute("ortho-size").as_float(10.0f),
            node.attribute("primary").as_bool(false),
        };
        scene->attachComponent<CameraComponent>(entity, params);
    }

    void SceneLoader::createScriptComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        ScriptComponentParams params{
            node.attribute("scriptName").as_string()
        };
        scene->attachComponent<ScriptComponent>(entity, params);
    }

    glm::vec3 SceneLoader::stringTupleToVec3(const std::string &s) {
        size_t p0 = 0;
        size_t p1 = s.find(' ');
        float x = std::stof(s.substr(p0, p1));
        p0 = p1 + 1;
        p1 = s.find(' ', p0);
        float y = std::stof(s.substr(p0, p1));
        float z = std::stof(s.substr(p1 + 1));

        return {x, y, z};
    }
}
