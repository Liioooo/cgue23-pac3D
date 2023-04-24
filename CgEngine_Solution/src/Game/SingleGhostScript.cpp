#include "SingleGhostScript.h"

namespace Game {
    void SingleGhostScript::onCollisionEnter(CgEngine::Entity other) {
        CG_LOGGING_INFO("Ghost {0}", other)
    }
}