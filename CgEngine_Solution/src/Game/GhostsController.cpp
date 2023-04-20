#include "GhostsController.h"
#include "glm/gtx/hash.hpp"

namespace Game {
    void GhostsController::onAttach() {
        /*
         * 36--32--------------29--35
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
         * |    |               |   |
         * 37--33--------------28--34
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
        mapNodes.emplace_back(glm::vec3(17.0f, 1.0f, 51.5f));   // 19
        mapNodes.emplace_back(glm::vec3(17.0f, 8.5f, -54.0f));  // 20
        mapNodes.emplace_back(glm::vec3(0.0f, 1.0f, -32.0f));   // 21
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, -32.0f)); // 22
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, 32.0f));  // 23
        mapNodes.emplace_back(glm::vec3(-17.0f, 8.5f, -54.0f)); // 24
        mapNodes.emplace_back(glm::vec3(-17.0f, 1.0f, 51.5f));  // 25
        mapNodes.emplace_back(glm::vec3(31.5f, 1.0f, 51.5f));   // 26
        mapNodes.emplace_back(glm::vec3(31.5f, 8.5f, -54.0f));  // 27
        mapNodes.emplace_back(glm::vec3(17.0f, 1.0f, 66.0f));   // 28
        mapNodes.emplace_back(glm::vec3(17.0f, 8.5f, -66.0f));  // 29
        mapNodes.emplace_back(glm::vec3(-31.5f, 8.5f, -54.0f)); // 30
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, 51.5f));  // 31
        mapNodes.emplace_back(glm::vec3(-17.0f, 8.5f, -66.0f)); // 32
        mapNodes.emplace_back(glm::vec3(-17.0f, 1.0f, 66.0f));  // 33
        mapNodes.emplace_back(glm::vec3(31.5f, 1.0f, 66.0f));   // 34
        mapNodes.emplace_back(glm::vec3(31.5f, 8.5f, -66.0f));  // 35
        mapNodes.emplace_back(glm::vec3(-31.5f, 8.5f, -66.0f)); // 36
        mapNodes.emplace_back(glm::vec3(-31.5f, 1.0f, 66.0f));  // 37

        setMapNeighbors(0, {1, 2});
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
        setMapNeighbors(19, {11, 25, 26, 28});
        setMapNeighbors(20, {12, 24, 27, 29});
        setMapNeighbors(21, {12, 14});
        setMapNeighbors(22, {13, 14, 30});
        setMapNeighbors(23, {13, 15, 31});
        setMapNeighbors(24, {14, 20, 30, 32});
        setMapNeighbors(25, {15, 19, 31, 33});
        setMapNeighbors(26, {17, 19, 34});
        setMapNeighbors(27, {18, 20, 35});
        setMapNeighbors(28, {19, 33, 34});
        setMapNeighbors(29, {20, 32, 35});
        setMapNeighbors(30, {22, 24, 36});
        setMapNeighbors(31, {23, 25, 37});
        setMapNeighbors(32, {24, 29, 36});
        setMapNeighbors(33, {25, 28, 37});
        setMapNeighbors(34, {26, 28});
        setMapNeighbors(35, {27, 29});
        setMapNeighbors(36, {30, 32});
        setMapNeighbors(37, {31, 33});
    }

    void GhostsController::update(CgEngine::TimeStep ts) {
        debugDrawMap();
    }

    void GhostsController::setMapNeighbors(size_t node, std::initializer_list<size_t> neighbors) {
        for (const auto& n: neighbors) {
            mapNodes[node].neighbors.push_back(mapNodes[n]);
        }
    }

    void GhostsController::debugDrawMap() {
        std::unordered_set<glm::vec3> visited;

        for (const auto& n1: mapNodes) {
            if (visited.count(n1.pos) != 0) {
                continue;
            }
            visited.insert(n1.pos);
            for (const auto& n2: n1.neighbors) {
                drawDebugLine(n1.pos, n2.pos, {1.0f, 0.0f, 0.0f});
            }
        }


    }
}
