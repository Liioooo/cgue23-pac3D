#pragma once

#include "Scripting/NativeScript.h"
#include "Logging.h"

using namespace CgEngine;

namespace Game {

    struct MapNode {
        glm::vec3 pos;
        std::vector<int> neighbors;

        explicit MapNode(glm::vec3 pos) : pos(pos) {};
    };

    class GhostsController : public NativeScript {
    public:
        static std::shared_ptr<CgEngine::NativeScript> instantiateScript() {
            return std::make_shared<GhostsController>();
        }

        void onAttach() override;
        void update(TimeStep ts) override;

    private:
        std::vector<MapNode> mapNodes;

        void setMapNeighbors(size_t node, std::initializer_list<int> neighbors);
        void createCoins();
        void createCoinAtPos(glm::vec3 pos);
        glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t);

        void debugDrawMap();
    };

}
