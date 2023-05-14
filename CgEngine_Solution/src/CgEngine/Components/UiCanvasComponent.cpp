#include "FileSystem.h"
#include "UiCanvasComponent.h"
#include "GlobalObjectManager.h"
#include "Ui/UiCircle.h"
#include "Asserts.h"

namespace CgEngine {
    void UiCanvasComponent::onAttach(Scene& scene, UiCanvasComponentParams& params) {
        for (const auto& element: params.elements) {
            std::string name = element.name();
            if (name == "UiCircle") {
                createElementCircle(element);
            } else if (name == "UiRect") {
                createElementRect(element);
            }
        }
    }

    void UiCanvasComponent::update(uint32_t viewportWidth, uint32_t viewportHeight) {
        if (viewportWidth == 0 && viewportHeight == 0) {
            return;
        }

        bool viewportDirty = viewportWidth != vWidth || viewportHeight != vHeight;
        vWidth = viewportWidth;
        vHeight = viewportHeight;

        for (const auto& element: uiElements) {
            element.second->update(viewportWidth, viewportHeight, viewportDirty);
        }
    }

    UiCircle* UiCanvasComponent::addUiCircle(const std::string& id) {
        auto* element = new UiCircle();
        uiElements.insert({id, element});
        return element;
    }

    UiRect* UiCanvasComponent::addUiRect(const std::string& id) {
        auto* element = new UiRect();
        uiElements.insert({id, element});
        return element;
    }

    void UiCanvasComponent::removeUIElement(const std::string& id) {
        if (uiElements.count(id) != 0) {
            auto* element = uiElements.at(id);
            delete element;
            uiElements.erase(id);
        }
    }

    const std::unordered_map<std::string, UiElement*>& UiCanvasComponent::getUiElements() const {
        return uiElements;
    }

    void UiCanvasComponent::createElementCircle(const pugi::xml_node& elementNode) {
        auto* element = createElement<UiCircle>(elementNode);

        element->setLineWidth(elementNode.attribute("line-width").as_float(0.0f));
        element->setLineColor(stringTupleToVec4(elementNode.attribute("line-color").as_string("0 0 0 1")));
        element->setFillColor(stringTupleToVec4(elementNode.attribute("fill-color").as_string("0 0 0 1")));

        std::string textureName = elementNode.attribute("texture").as_string("");
        if (!textureName.empty()) {
            std::string texturePath = FileSystem::getAsGamePath(textureName);
            auto& resourceManager = GlobalObjectManager::getInstance().getResourceManager();
            element->setTexture(resourceManager.getResource<Texture2D>(texturePath));
        }

        element->setWidth(elementNode.attribute("width").as_float(100.0f));
    }

    void UiCanvasComponent::createElementRect(const pugi::xml_node& elementNode) {
        auto* element = createElement<UiRect>(elementNode);

        element->setLineWidth(elementNode.attribute("line-width").as_float(0.0f));
        element->setLineColor(stringTupleToVec4(elementNode.attribute("line-color").as_string("0 0 0 1")));
        element->setFillColor(stringTupleToVec4(elementNode.attribute("fill-color").as_string("0 0 0 1")));

        std::string textureName = elementNode.attribute("texture").as_string("");
        if (!textureName.empty()) {
            std::string texturePath = FileSystem::getAsGamePath(textureName);
            auto& resourceManager = GlobalObjectManager::getInstance().getResourceManager();
            element->setTexture(resourceManager.getResource<Texture2D>(texturePath));
        }

        element->setWidth(elementNode.attribute("width").as_float(100.0f));
        element->setHeight(elementNode.attribute("height").as_float(100.0f));
    }

    template<typename E>
    E* UiCanvasComponent::createElement(const pugi::xml_node& elementNode) {
        std::string id = elementNode.attribute("id").as_string("");

        CG_ASSERT(!id.empty(), "ID of UIElement must be set!")
        CG_ASSERT(uiElements.count(id) == 0, "ID of UIElement already used!")

        E* element = new E();
        uiElements.insert({id, element});

        auto top = stringToPosAndUnit(elementNode.attribute("top").as_string("0px"));
        auto left = stringToPosAndUnit(elementNode.attribute("left").as_string("0px"));
        auto bottom = stringToPosAndUnit(elementNode.attribute("bottom").as_string("-1px"));
        auto right = stringToPosAndUnit(elementNode.attribute("right").as_string("-1px"));

        element->setTop(top.first, top.second);
        element->setLeft(left.first, left.second);
        element->setBottom(bottom.first, bottom.second);
        element->setRight(right.first, right.second);

        element->setXAlignment(stringToXAlignment(elementNode.attribute("x-align").as_string("center")));
        element->setYAlignment(stringToYAlignment(elementNode.attribute("y-align").as_string("center")));

        element->setZIndex(elementNode.attribute("z-index").as_int(0));

        return element;
    }

    std::pair<float, UIPosUnit> UiCanvasComponent::stringToPosAndUnit(const std::string& s) {
        if (s.length() > 2 && s.compare(s.length() - 2, 2, "px") == 0) {
            return {std::stof(s.substr(0, s.length() - 2)), UIPosUnit::Pixel};
        }
        return {std::stof(s), UIPosUnit::Percent};
    }

    UIXAlignment UiCanvasComponent::stringToXAlignment(const std::string& s) {
        if (s == "left") {
            return UIXAlignment::Left;
        } else if (s == "right") {
            return UIXAlignment::Right;
        }
        return UIXAlignment::Center;
    }

    UIYAlignment UiCanvasComponent::stringToYAlignment(const std::string& s) {
        if (s == "top") {
            return UIYAlignment::Top;
        } else if (s == "bottom") {
            return UIYAlignment::Bottom;
        }
        return UIYAlignment::Center;
    }

    glm::vec4 UiCanvasComponent::stringTupleToVec4(const std::string& s) {
        size_t p0 = 0;
        size_t p1 = s.find(' ');
        float x = std::stof(s.substr(p0, p1));
        p0 = p1 + 1;
        p1 = s.find(' ', p0);
        float y = std::stof(s.substr(p0, p1));
        p1++;
        p0 = s.find(' ', p1);
        float z = std::stof(s.substr(p1, p0));
        float w = std::stof(s.substr(p0 + 1));

        return {x, y, z, w};
    }

}