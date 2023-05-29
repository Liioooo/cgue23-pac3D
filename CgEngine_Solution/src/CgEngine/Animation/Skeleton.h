#pragma once

namespace CgEngine {

    class Skeleton {
    public:
        static const uint32_t NoBone = ~0;

        Skeleton(uint32_t boneCount);

        uint32_t addBone(const std::string& name, uint32_t parent, const glm::mat4& transform);
        uint32_t findBoneIndex(const std::string_view name) const;
        uint32_t getParentBoneIndex(uint32_t index) const;
        const std::string& getBoneName(uint32_t index) const;
        const glm::mat4& getBoneTransform(uint32_t index) const;

        uint32_t getNumBones() const;

    private:
        std::vector<std::string> boneNames;
        std::vector<uint32_t> parentBoneIndices;
        std::vector<glm::mat4> boneTransforms;
    };

}
