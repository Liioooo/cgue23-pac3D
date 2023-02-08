#pragma once

#include "Event.h"

namespace CgEngine {

    class KeyReleasedEvent : public Event {
    public:
        KeyReleasedEvent(const int keyCode, const int modifierKeys) : keyCode(keyCode), modifierKeys(modifierKeys) {};

        inline int getKeyCode() const {
            return keyCode;
        }
        inline int getModifierKeys() const {
            return modifierKeys;
        }

        EVENT_TYPE_FN(KeyReleased);

    private:
        int keyCode;
        int modifierKeys;
    };

}
