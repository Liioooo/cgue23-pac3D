#pragma once

#include "Event.h"

namespace CgEngine {

    class MouseButtonPressedEvent : public Event {
    public:
        MouseButtonPressedEvent(const MouseButton button) : button(button) {}

        inline MouseButton getButton() const {
            return button;
        }

        EVENT_TYPE_FN(MouseButtonPressed);

    private:
        MouseButton button;
    };

}
