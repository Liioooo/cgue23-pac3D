#pragma once

namespace CgEngine {

    enum class UIElementType {
        Circle
    };

    enum class UIPosUnit {
        Pixel, Percent
    };

    enum class UIXAlignment {
        Left, Center, Right
    };

    enum class UIYAlignment {
        Top, Center, Bottom
    };

    class UiElement {
    public:
        explicit UiElement(UIElementType type);
        virtual ~UiElement() = default;

        inline UIElementType getType() const {
            return type;
        }

        void setTop(float top, UIPosUnit unit);
        void setRight(float right, UIPosUnit unit);
        void setBottom(float bottom, UIPosUnit unit);
        void setLeft(float left, UIPosUnit unit);

        void setXAlignment(UIXAlignment xAlignment);
        void setYAlignment(UIYAlignment yAlignment);

        uint32_t getZIndex() const;
        void setZIndex(uint32_t zIndex);

        void update(const glm::mat3& entityTransform, uint32_t viewportWidth, uint32_t viewportHeight, bool viewportDirty);

        virtual const std::vector<glm::vec4>& getVertices() const = 0;

    protected:
        virtual void updateVertices(const glm::mat3& entityTransform, bool absolutePosDirty) = 0;

        UIXAlignment xAlignment;
        UIYAlignment yAlignment;
        glm::vec2 absolutePos = glm::vec2(0.0f, 0.0f);

    private:
        const UIElementType type;

        std::pair<float, UIPosUnit> top;
        std::pair<float, UIPosUnit> right;
        std::pair<float, UIPosUnit> bottom;
        std::pair<float, UIPosUnit> left;

        uint32_t zIndex = 0;
        bool dirty = true;
    };

}
