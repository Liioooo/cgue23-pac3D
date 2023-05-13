#pragma once

#include "UiElement.h"
#include "Rendering/Texture.h"

namespace CgEngine {

    class UiCircle : public UiElement {
    public:
        explicit UiCircle() : UiElement(UIElementType::Circle) {};
        ~UiCircle() override = default;

        void setWidth(uint32_t width);
        void setHeight(uint32_t height);

        void setLineWidth(float lineWidth);
        void setLineColor(const glm::vec4& lineColor);
        void setFillColor(const glm::vec4& fillColor);
        void setTexture(Texture2D* texture);

        float getLineWidth() const;
        const glm::vec4& getLineColor() const;
        const glm::vec4& getFillColor() const;
        const Texture2D* getTexture() const;

        const std::vector<glm::vec4>& getVertices() const override;

    protected:
        void updateVertices(const glm::mat3& entityTransform, bool absolutePosDirty) override;

    private:
        uint32_t width{};
        uint32_t height{};

        float lineWidth{};
        glm::vec4 lineColor{};
        glm::vec4 fillColor{};

        Texture2D* texture = nullptr;

        bool dirty = true;

        std::vector<glm::vec4> vertices;
    };

}