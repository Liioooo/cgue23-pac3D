#pragma once

namespace CgEngine {

    class AABoundingBox {
    public:
        AABoundingBox() = default;

        void addBoxCoordinates(glm::vec3 max, glm::vec3 min);
        void applyTransform(const glm::mat4& transform);

        glm::mat4 getTransformForCubeMesh() const;

    private:
        glm::vec3 min;
        glm::vec3 max;

        bool addedCoords = false;
    };

}
