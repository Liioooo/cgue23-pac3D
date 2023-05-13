#pragma once

#include "Component.h"
#include "pugixml.hpp"
#include "Ui/UiCircle.h"

namespace CgEngine {

    struct UiCanvasComponentParams {
        const pugi::xml_object_range<pugi::xml_node_iterator>& elements;
    };

    class UiCanvasComponent : public Component {
    public:
        using Component::Component;

        void onAttach(Scene& scene, UiCanvasComponentParams& params);

        void update(const glm::mat4& entityTransform, uint32_t viewportWidth, uint32_t viewportHeight);

        UiCircle* addCircleUi(const std::string& id);

        void removeUIElement(const std::string& id);

        template<typename U>
        U* getUIElement(const std::string& id) {
            return dynamic_cast<U*>(uiElements.at(id));
        }

        const std::unordered_map<std::string, UiElement*>& getUiElements() const;

    private:
        std::unordered_map<std::string, UiElement*> uiElements;
        uint32_t vWidth = 0;
        uint32_t vHeight = 0;

        void createElementCircle(const pugi::xml_node& elementNode);

        std::pair<float, UIPosUnit> stringToPosAndUnit(const std::string& s);
        UIXAlignment stringToXAlignment(const std::string& s);
        UIYAlignment stringToYAlignment(const std::string& s);
        glm::vec4 stringTupleToVec4(const std::string& s);

    };

}