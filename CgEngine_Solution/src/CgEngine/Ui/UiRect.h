#pragma once

#include "UiElement.h"
#include "Rendering/Texture.h"

namespace CgEngine {

    class UiRect : public UiElement {
    public:
        explicit UiRect() : UiElement(UIElementType::Rect) {};
        ~UiRect() override = default;

        void setWidth(float width);
        void setHeight(float height);

        void setLineWidth(float lineWidth);
        void setLineColor(const glm::vec4& lineColor);
        void setFillColor(const glm::vec4& fillColor);
        void setTexture(Texture2D* texture);

        float getLineWidth() const;
        const glm::vec4& getLineColor() const;
        const glm::vec4& getFillColor() const;
        const Texture2D* getTexture() const;

        const glm::vec2& getSize() const;

        const std::vector<glm::vec4>& getVertices() const override;

    protected:
        void updateElement(bool absolutePosDirty) override;

    private:
        glm::vec2 size;

        float lineWidth{};
        glm::vec4 lineColor{};
        glm::vec4 fillColor{};

        Texture2D* texture = nullptr;

        bool dirty = true;

        std::vector<glm::vec4> vertices;
    };

}