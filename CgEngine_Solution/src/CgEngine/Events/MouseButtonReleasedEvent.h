#pragma once

#include "Event.h"

namespace CgEngine {

    class MouseButtonReleasedEvent : public Event {
    public:
        MouseButtonReleasedEvent(const int button) : button(button) {}

        inline int getButton() const {
            return button;
        }

        EVENT_TYPE_FN(MouseButtonReleased);

    private:
        int button;
    };

}
