#include "PhysicsCooking.h"
#include "GlobalObjectManager.h"
#include "Asserts.h"

namespace CgEngine {
    PhysicsCooking::PhysicsCooking(physx::PxFoundation* pxFoundation, const physx::PxTolerancesScale& tolerancesScale) {
        auto params = physx::PxCookingParams(tolerancesScale);
        physxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *pxFoundation, params);
        CG_ASSERT(physxCooking, "Error while creating PhysXCooking")
    }

    PhysicsCooking::~PhysicsCooking() {
        physxCooking->release();
        physxCooking = nullptr;
    }

    PhysicsTriangleMesh* PhysicsCooking::cookTriangleMesh(glm::vec3* vertices, uint32_t numVertices, uint32_t* indices, uint32_t indexCount) {
        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = numVertices;
        meshDesc.points.stride = sizeof(glm::vec3);
        meshDesc.points.data = vertices;

        meshDesc.triangles.count = indexCount / 3;
        meshDesc.triangles.stride = 3 * sizeof(uint32_t);
        meshDesc.triangles.data = indices;

        physx::PxDefaultMemoryOutputStream writeBuffer;
        physx::PxTriangleMeshCookingResult::Enum result;
        bool status = physxCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

        CG_ASSERT(status, "Failed to cook TriangleMesh")

        auto& physxPhysics = GlobalObjectManager::getInstance().getPhysicsSystem().getPhysxPhysics();

        physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

        return new PhysicsTriangleMesh(physxPhysics.createTriangleMesh(readBuffer));
    }
}