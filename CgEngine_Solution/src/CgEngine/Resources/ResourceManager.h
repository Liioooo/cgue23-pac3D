#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <Resources/XMLFile.h>
#include <Resources/MeshVertices.h>
#include <Rendering/Shader.h>
#include "Rendering/Material.h"
#include "Rendering/Texture.h"

namespace CgEngine {

    template<typename R>
    class ResourceMap {
    public:
        R* get(const std::string& name) const {
            return map.at(name).get();
        }

        void insert(const std::string& name, R* resource) {
            map.insert({name, std::unique_ptr<R>(resource)});
        }

        bool contains(const std::string& name) const {
            return map.count(name) != 0;
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<R>> map{};
    };

    class ResourceManager {
    public:
        ResourceManager() {
            registerResourceType<XMLFile>();
            registerResourceType<MeshVertices>();
            registerResourceType<Shader>();
            registerResourceType<Material>();
            registerResourceType<Texture2D>();
        }

        template<typename R>
        R* getResource(const std::string& name) {
            std::shared_ptr<ResourceMap<R>> resourceMap = getResourceMap<R>();
            if (resourceMap->contains(name)) {
                return resourceMap->get(name);
            }
            R* resource = R::createResource(name);
            resourceMap->insert(name, resource);
            return resource;
        }

        template<typename R>
        bool hasResource(const std::string& name) {
            std::shared_ptr<ResourceMap<R>> resourceMap = getResourceMap<R>();
            return resourceMap->contains(name);
        }

        template<typename R>
        bool insertResource(const std::string& name, R* resource) {
            std::shared_ptr<ResourceMap<R>> resourceMap = getResourceMap<R>();
            if (resourceMap->contains(name)) {
                return false;
            }
            resourceMap->insert(name, resource);
            return true;
        }

    private:
        std::unordered_map<const char*, std::shared_ptr<void>> resourceMaps;

        template<typename R>
        void registerResourceType() {
            const char* typeName = typeid(R).name();
            resourceMaps.insert({typeName, std::make_shared<ResourceMap<R>>()});
        }

        template<typename R>
        std::shared_ptr<ResourceMap<R>> getResourceMap() {
            const char* typeName = typeid(R).name();
            return std::static_pointer_cast<ResourceMap<R>>(resourceMaps.at(typeName));
        }
    };

}
