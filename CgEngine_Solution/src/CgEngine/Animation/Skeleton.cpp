#include "Skeleton.h"

namespace CgEngine {
    Skeleton::Skeleton(uint32_t boneCount) {
        boneNames.reserve(boneCount);
        parentBoneIndices.reserve(boneCount);
        boneTransforms.reserve(boneCount);
    }

    uint32_t Skeleton::addBone(const std::string& name, uint32_t parent, const glm::mat4& transform) {
        uint32_t boneIndex = boneNames.size();
        boneNames.emplace_back(name);
        parentBoneIndices.emplace_back(parent);
        boneTransforms.push_back(transform);
        return boneIndex;
    }

    uint32_t Skeleton::findBoneIndex(const std::string_view name) const {
        for (uint32_t i = 0; i < boneNames.size(); i++) {
            if (boneNames[i] == name) {
                return i;
            }
        }
        return Skeleton::NoBone;
    }

    uint32_t Skeleton::getParentBoneIndex(uint32_t index) const {
        return parentBoneIndices.at(index);
    }

    const std::string& Skeleton::getBoneName(uint32_t index) const {
        return boneNames.at(index);
    }

    const glm::mat4& Skeleton::getBoneTransform(uint32_t index) const {
        return boneTransforms.at(index);
    }

    uint32_t Skeleton::getNumBones() const {
        return boneNames.size();
    }
}