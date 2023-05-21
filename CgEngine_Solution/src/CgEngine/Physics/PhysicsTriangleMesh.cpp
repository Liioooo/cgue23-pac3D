#include "PhysicsTriangleMesh.h"

namespace CgEngine {
    physx::PxTriangleMesh* PhysicsTriangleMesh::getPhysxMesh() {
        return mesh;
    }
}