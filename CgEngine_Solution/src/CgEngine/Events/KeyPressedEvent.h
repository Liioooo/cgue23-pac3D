#pragma once

#include "Event.h"

namespace CgEngine {

    class KeyPressedEvent : public Event {
    public:
        KeyPressedEvent(const int keyCode, const int modifierKeys) : keyCode(keyCode), modifierKeys(modifierKeys) {};

        inline int getKeyCode() const {
            return keyCode;
        }
        inline int getModifierKeys() const {
            return modifierKeys;
        }

        EVENT_TYPE_FN(KeyPressed);

    private:
        int keyCode;
        int modifierKeys;
    };

}
