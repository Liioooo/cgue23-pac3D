#include "UiCircle.h"

namespace CgEngine {
    void UiCircle::setWidth(float width) {
        this->width = width;
        dirty = true;
    }

    float UiCircle::getWidth() const {
        return width;
    }

    void UiCircle::setLineWidth(float lineWidth) {
        this->lineWidth = lineWidth;
    }

    void UiCircle::setLineColor(const glm::vec4& lineColor) {
        this->lineColor = lineColor;
    }

    void UiCircle::setFillColor(const glm::vec4& fillColor) {
        this->fillColor = fillColor;
    }

    void UiCircle::setTexture(CgEngine::Texture2D* texture) {
        this->texture = texture;
    }

    float UiCircle::getLineWidth() const {
        return lineWidth;
    }

    const glm::vec4& UiCircle::getLineColor() const {
        return lineColor;
    }

    const glm::vec4& UiCircle::getFillColor() const {
        return fillColor;
    }

    const Texture2D* UiCircle::getTexture() const {
        return texture;
    }

    const std::vector<glm::vec4>& UiCircle::getVertices() const {
        return vertices;
    }

    void UiCircle::updateElement(bool absolutePosDirty) {
        if (dirty || absolutePosDirty) {
            vertices.clear();

            vertices.emplace_back(0.0f, 0.0f, 0.0f, 0.0f);
            vertices.emplace_back(width, 0.0f, 1.0f, 0.0f);
            vertices.emplace_back(width, width, 1.0f, 1.0f);
            vertices.emplace_back(0.0f, width, 0.0f, 1.0f);

            for (auto& vertex: vertices) {
                switch (xAlignment) {
                    case UIXAlignment::Left:
                        vertex.x += absolutePos.x;
                        break;
                    case UIXAlignment::Right:
                        vertex.x += absolutePos.x - width;
                        break;
                    case UIXAlignment::Center:
                        vertex.x += absolutePos.x - width / 2;
                        break;
                }

                switch (yAlignment) {
                    case UIYAlignment::Top:
                        vertex.y += absolutePos.y - width;
                        break;
                    case UIYAlignment::Bottom:
                        vertex.y += absolutePos.y;
                        break;
                    case UIYAlignment::Center:
                        vertex.y += absolutePos.y - width / 2;
                        break;
                }
            }
        }

        dirty = false;
    }
}