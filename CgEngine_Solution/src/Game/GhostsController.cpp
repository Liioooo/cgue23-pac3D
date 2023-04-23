#include "GhostsController.h"
#include "glm/gtx/hash.hpp"

namespace Game {
    void GhostsController::onAttach() {
        /*
         * 28--32--------------29--33
         * |    |               |   |
         * 30--24--------------20--27
         * |    |               |   |
         * 22--14-----21-------12--18
         * |    |      |        |   |
         * |    |  2---0---1    |   |
         * |    |  |       |    |   |
         * 13---8--4-------3----6--10
         * |    |  |       |    |   |
         * |    |  7---9---5    |   |
         * |    |      |        |   |
         * 23--15-----16-------11--17
         * |    |               |   |
         * 31--25--------------19--26
         */

        mapNodes.emplace_back(glm::vec3(0.0f, 1.0f, -17.0f));   // 0
        mapNodes.emplace_back(glm::vec3(9.0f, 1.0f, -17.0f));   // 1
        mapNodes.emplace_back(glm::vec3(-9.0f, 1.0f, -17.0f));  // 2
        mapNodes.emplace_back(glm::vec3(9.0f, 1.0f, 0.0f));     // 3
        mapNodes.emplace_back(glm::vec3(-9.0f, 1.0f, 0.0f));    // 4
        mapNodes.emplace_back(glm::vec3(9.0f, 1.0f, 12.0f));    // 5
        mapNodes.emplace_back(glm::vec3(17.0f, 1.0f, 0.0f));    // 6
        mapNodes.emplace_back(glm::vec3(-9.0f, 1.0f, 12.0f));   // 7
        mapNodes.emplace_back(glm::vec3(-17.0f, 1.0f, 0.0f));   // 8
        mapNodes.emplace_back(glm::vec3(0.0f, 1.0f, 12.0f));    // 9
        mapNodes.emplace_back(glm::vec3(31.5f, 1.0f, 0.0f));    // 10
        mapNodes.emplace_back(glm::vec3(17.0f, 1.0f, 32.0f));   // 11
        mapNodes.emplace_back(glm::vec3(17.0f, 1.0f, -32.0f));  // 12
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, 0.0f));   // 13
        mapNodes.emplace_back(glm::vec3(-17.0f, 1.0f, -32.0f)); // 14
        mapNodes.emplace_back(glm::vec3(-17.0f, 1.0f, 32.0f));  // 15
        mapNodes.emplace_back(glm::vec3(0.0f, 1.0f, 32.0f));    // 16
        mapNodes.emplace_back(glm::vec3(31.5f, 1.0f, 32.0f));   // 17
        mapNodes.emplace_back(glm::vec3(31.5f, 1.0f, -32.0f));  // 18
        mapNodes.emplace_back(glm::vec3(17.0f, 1.0f, 46.5f));   // 19
        mapNodes.emplace_back(glm::vec3(17.0f, 8.5f, -54.0f));  // 20
        mapNodes.emplace_back(glm::vec3(0.0f, 1.0f, -32.0f));   // 21
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, -32.0f)); // 22
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, 32.0f));  // 23
        mapNodes.emplace_back(glm::vec3(-17.0f, 8.5f, -54.0f)); // 24
        mapNodes.emplace_back(glm::vec3(-17.0f, 1.0f, 46.5f));  // 25
        mapNodes.emplace_back(glm::vec3(31.5f, 1.0f, 46.5f));   // 26
        mapNodes.emplace_back(glm::vec3(31.5f, 8.5f, -54.0f));  // 27
        mapNodes.emplace_back(glm::vec3(-31.5f, 8.5f, -66.0f)); // 28
        mapNodes.emplace_back(glm::vec3(17.0f, 8.5f, -66.0f));  // 29
        mapNodes.emplace_back(glm::vec3(-31.5f, 8.5f, -54.0f)); // 30
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, 46.5f));  // 31
        mapNodes.emplace_back(glm::vec3(-17.0f, 8.5f, -66.0f)); // 32
        mapNodes.emplace_back(glm::vec3(31.5f, 8.5f, -66.0f));  // 33

        setMapNeighbors(0, {1, 2, 21});
        setMapNeighbors(1, {0, 3});
        setMapNeighbors(2, {0, 4});
        setMapNeighbors(3, {1, 4, 5, 6});
        setMapNeighbors(4, {2, 3, 7, 8});
        setMapNeighbors(5, {3, 9});
        setMapNeighbors(6, {3, 10, 11, 12});
        setMapNeighbors(7, {4, 9});
        setMapNeighbors(8, {4, 13, 14, 15});
        setMapNeighbors(9, {5, 7, 16});
        setMapNeighbors(10, {6, 17, 18});
        setMapNeighbors(11, {6, 16, 17, 19});
        setMapNeighbors(12, {6, 18, 20, 21});
        setMapNeighbors(13, {8, 22, 23});
        setMapNeighbors(14, {21, 22, 24});
        setMapNeighbors(15, {8, 16, 23, 25});
        setMapNeighbors(16, {11, 15});
        setMapNeighbors(17, {10, 11, 26});
        setMapNeighbors(18, {10, 12, 27});
        setMapNeighbors(19, {11, 25, 26});
        setMapNeighbors(20, {12, 24, 27, 29});
        setMapNeighbors(21, {0, 12, 14});
        setMapNeighbors(22, {13, 14, 30});
        setMapNeighbors(23, {13, 15, 31});
        setMapNeighbors(24, {14, 20, 30, 32});
        setMapNeighbors(25, {15, 19, 31});
        setMapNeighbors(26, {17, 19});
        setMapNeighbors(27, {18, 20, 33});
        setMapNeighbors(28, {30, 32});
        setMapNeighbors(29, {20, 32, 33});
        setMapNeighbors(30, {22, 24, 28});
        setMapNeighbors(31, {23, 25});
        setMapNeighbors(32, {24, 28, 29});
        setMapNeighbors(33, {27, 29});

        createCoins();
    }

    void GhostsController::update(CgEngine::TimeStep ts) {
    }

    void GhostsController::setMapNeighbors(size_t node, std::initializer_list<int> neighbors) {
        for (const auto& n: neighbors) {
            mapNodes[node].neighbors.push_back(n);
        }
    }

    void GhostsController::createCoins() {
        std::unordered_set<std::string> visitedEdges;
        for (int i = 0; i < mapNodes.size(); ++i) {
            createCoinAtPos(mapNodes[i].pos);
            for (int j = 0; j < mapNodes[i].neighbors.size(); ++j) {
                int jIndex = mapNodes[i].neighbors[j];
                std::string edgeKey = i < jIndex ? std::to_string(i)  + ":" + std::to_string(jIndex) : std::to_string(jIndex)  + ":" + std::to_string(i);
                if (visitedEdges.count(edgeKey) != 0) {
                    continue;
                }
                visitedEdges.insert(edgeKey);

                float edgeLength = glm::distance(mapNodes[i].pos, mapNodes[jIndex].pos);
                int coinAmount = static_cast<int>(glm::round(edgeLength / 1.5f));

                for (int c = 1; c < coinAmount; c++) {
                    createCoinAtPos(lerp(mapNodes[i].pos, mapNodes[jIndex].pos, static_cast<float>(c) / static_cast<float>(coinAmount)));
                }
            }
        }
    }

    void GhostsController::createCoinAtPos(glm::vec3 pos) {
        Entity e = createEntity();

        pos.y -= 0.5f;

        CgEngine::TransformComponentParams transformParams{
            pos, glm::vec3(0.0f), glm::vec3(0.1f)
        };
        CgEngine::MeshRendererComponentParams rendererParams{
            "", "CG_SphereMesh", "Red", true
        };
        CgEngine::SphereColliderComponentParams colliderParams{
            1.4f, glm::vec3(0.0f), true, "default-physics-material"
        };
        CgEngine::RigidBodyComponentParams rigidBodyParams{};
        rigidBodyParams.isDynamic = false;

        attachComponent<CgEngine::TransformComponent>(e, transformParams);
        attachComponent<CgEngine::MeshRendererComponent>(e, rendererParams);
        attachComponent<CgEngine::SphereColliderComponent>(e, colliderParams);
        attachComponent<CgEngine::RigidBodyComponent>(e, rigidBodyParams);
    }

    glm::vec3 GhostsController::lerp(glm::vec3 x, glm::vec3 y, float t) {
        return x * (1.0f - t) + y * t;
    }

    void GhostsController::debugDrawMap() {
        for (const auto& n1: mapNodes) {
            for (const auto& n2: n1.neighbors) {
                drawDebugLine(n1.pos, mapNodes[n2].pos, {1.0f, 0.0f, 0.0f});
            }
        }


    }
}
