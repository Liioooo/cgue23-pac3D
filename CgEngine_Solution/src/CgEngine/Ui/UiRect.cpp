#include "UiRect.h"

namespace CgEngine {
    void UiRect::setWidth(float width) {
        size.x = width;
        dirty = true;
    }

    void UiRect::setHeight(float height) {
        size.y = height;
        dirty = true;
    }

    void UiRect::setLineWidth(float lineWidth) {
        this->lineWidth = lineWidth;
    }

    void UiRect::setLineColor(const glm::vec4& lineColor) {
        this->lineColor = lineColor;
    }

    void UiRect::setFillColor(const glm::vec4& fillColor) {
        this->fillColor = fillColor;
    }

    void UiRect::setTexture(CgEngine::Texture2D* texture) {
        this->texture = texture;
    }

    float UiRect::getLineWidth() const {
        return lineWidth;
    }

    const glm::vec4& UiRect::getLineColor() const {
        return lineColor;
    }

    const glm::vec4& UiRect::getFillColor() const {
        return fillColor;
    }

    const Texture2D* UiRect::getTexture() const {
        return texture;
    }

    const glm::vec2& UiRect::getSize() const {
        return size;
    }

    const std::vector<glm::vec4>& UiRect::getVertices() const {
        return vertices;
    }

    bool UiRect::isPointInside(const glm::vec2& point) const {
        return point.x >= collisionPoints[0] && point.x <= collisionPoints[2] && point.y >= collisionPoints[1] && point.y <= collisionPoints[3];
    }

    void UiRect::updateElement(bool absolutePosDirty, uint32_t viewportWidth, uint32_t viewportHeight) {
        if (dirty || absolutePosDirty) {
            vertices.clear();

            vertices.emplace_back(0.0f, 0.0f, 0.0f, 1.0f);
            vertices.emplace_back(size.x, 0.0f, 1.0f, 1.0f);
            vertices.emplace_back(size.x, size.y, 1.0f, 0.0f);
            vertices.emplace_back(0.0f, size.y, 0.0f, 0.0f);

            for (auto& vertex: vertices) {
                switch (xAlignment) {
                    case UIXAlignment::Left:
                        vertex.x += absolutePos.x;
                        break;
                    case UIXAlignment::Right:
                        vertex.x += absolutePos.x - size.x;
                        break;
                    case UIXAlignment::Center:
                        vertex.x += absolutePos.x - size.x / 2;
                        break;
                }

                switch (yAlignment) {
                    case UIYAlignment::Top:
                        vertex.y += absolutePos.y - size.y;
                        break;
                    case UIYAlignment::Bottom:
                        vertex.y += absolutePos.y;
                        break;
                    case UIYAlignment::Center:
                        vertex.y += absolutePos.y - size.y / 2;
                        break;
                }
            }
        }

        collisionPoints[0] = vertices[0].x;
        collisionPoints[1] = viewportHeight - vertices[2].y;
        collisionPoints[2] = vertices[2].x;
        collisionPoints[3] = viewportHeight - vertices[0].y;


        dirty = false;
    }
}