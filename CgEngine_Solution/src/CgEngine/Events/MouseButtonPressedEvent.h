#pragma once

#include "Event.h"

namespace CgEngine {

    class MouseButtonPressedEvent : public Event {
    public:
        MouseButtonPressedEvent(const int button) : button(button) {}

        inline int getButton() const {
            return button;
        }

        EVENT_TYPE_FN(MouseButtonPressed);

    private:
        float button;
    };

}
