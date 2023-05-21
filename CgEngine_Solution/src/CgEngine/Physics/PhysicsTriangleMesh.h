#pragma once

#include "PxPhysicsAPI.h"

namespace CgEngine {

    class PhysicsTriangleMesh {
    public:
        explicit PhysicsTriangleMesh(physx::PxTriangleMesh* mesh) : mesh(mesh) {}

        physx::PxTriangleMesh* getPhysxMesh();

    private:
        physx::PxTriangleMesh* mesh;
    };

}
