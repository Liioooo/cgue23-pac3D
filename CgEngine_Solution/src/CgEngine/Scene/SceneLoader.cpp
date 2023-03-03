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
        } else if (name == "DirectionalLightComponent") {
            createDirectionalLightComponent(scene, entity, node);
        } else if (name == "PointLightComponent") {
            createPointLightComponent(scene, entity, node);
        } else if (name == "SpotLightComponent") {
            createSpotLightComponent(scene, entity, node);
        } else if (name == "SkyboxComponent") {
            createSkyboxComponent(scene, entity, node);
        }
    }

     void SceneLoader::createTransformComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        glm::vec3 position = stringTupleToVec3(node.attribute("position").as_string());
        glm::vec3 rotation = stringTupleToVec3(node.attribute("rotation").as_string());
        glm::vec3 scale = stringTupleToVec3(node.attribute("scale").as_string());
        TransformComponentParams params{position, glm::radians(rotation), scale};
        scene->attachComponent<TransformComponent>(entity, params);
    }

    void SceneLoader::createMeshRendererComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        MeshRendererComponentParams params{
            node.attribute("asset-file").as_string(),
            node.attribute("mesh").as_string(),
            node.attribute("material").as_string(),
            getListFromString(node.attribute("submesh-indices").as_string())
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
            node.attribute("script-name").as_string()
        };
        scene->attachComponent<ScriptComponent>(entity, params);
    }

    void SceneLoader::createDirectionalLightComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        DirectionalLightComponentParams params{
                hexStringToColor(node.attribute("color").as_string()),
                node.attribute("intensity").as_float(1.0f),
                node.attribute("cast-shadows").as_bool(true)
        };
        scene->attachComponent<DirectionalLightComponent>(entity, params);
    }

    void SceneLoader::createPointLightComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        PointLightComponentParams params{
                hexStringToColor(node.attribute("color").as_string()),
                node.attribute("intensity").as_float(1.0f),
                node.attribute("radius").as_float(5.0f),
                node.attribute("falloff").as_float(1.0f),
        };
        scene->attachComponent<PointLightComponent>(entity, params);
    }

    void SceneLoader::createSpotLightComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        SpotLightComponentParams params{
                hexStringToColor(node.attribute("color").as_string()),
                node.attribute("intensity").as_float(1.0f),
                node.attribute("radius").as_float(5.0f),
                node.attribute("falloff").as_float(1.0f),
                glm::radians(node.attribute("inner-angle").as_float(30.0f)),
                glm::radians(node.attribute("outer-angle").as_float(35.0f)),
        };
        scene->attachComponent<SpotLightComponent>(entity, params);
    }

    void SceneLoader::createSkyboxComponent(Scene *scene, Entity entity, const pugi::xml_node &node) {
        SkyboxComponentParams params{
            node.attribute("hdri-path").as_string(),
            node.attribute("intensity").as_float(1.0f),
            node.attribute("lod").as_float(1.0f)
        };
        scene->attachComponent<SkyboxComponent>(entity, params);
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

    glm::vec3 SceneLoader::hexStringToColor(const std::string &s) {
        uint64_t color = std::stoul(s.substr(1), nullptr, 16);
        float r = ((color >> 16) & 0xFF) / 255.0f;
        float g = ((color >> 8) & 0xFF) / 255.0f;
        float b = (color & 0xFF) / 255.0f;
        return {r, g, b};
    }

    std::vector<uint32_t> SceneLoader::getListFromString(const std::string &s) {
        std::vector<uint32_t> result{};
        std::stringstream ss(s);
        std::string item;

        while (std::getline(ss, item, ',')) {
            result.emplace_back(std::stoi(item));
        }

        return result;
    }
}
