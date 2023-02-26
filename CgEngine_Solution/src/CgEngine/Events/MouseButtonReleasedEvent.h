#pragma once

#include "Event.h"
#include "KeyCodes.h"

namespace CgEngine {

    class MouseButtonReleasedEvent : public Event {
    public:
        MouseButtonReleasedEvent(const MouseButton button) : button(button) {}

        inline MouseButton getButton() const {
            return button;
        }

        EVENT_TYPE_FN(MouseButtonReleased);

    private:
        MouseButton button;
    };

}
