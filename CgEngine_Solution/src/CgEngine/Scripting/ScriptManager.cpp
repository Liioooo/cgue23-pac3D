#include "ScriptManager.h"
#include "Asserts.h"

namespace CgEngine {
    std::shared_ptr<NativeScript> ScriptManager::getScriptInstance(const std::string& name) {
        CG_ASSERT(scriptMap.count(name) != 0, "Script not registered: " + name);

        return scriptMap.at(name)();
    }
}
