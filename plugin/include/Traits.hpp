#pragma once

#include <iostream>

namespace plugin::traits {
    class Display {
        public:
        virtual ~Display(void) = default;
        virtual std::ostream& display(std::ostream& os) = 0;
    };

    std::ostream& operator<<(std::ostream& os, Display& display) {
        return display.display(os);
    }
}