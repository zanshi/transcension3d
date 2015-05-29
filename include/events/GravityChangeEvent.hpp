//
// Created by Adrian Palmquist on 2015-05-28.
//

#pragma once

namespace sw {
    struct GravityChangeEvent {
        GravityChangeEvent(bool gravityChange)
                : gravityChange_(gravityChange) {}

        bool gravityChange_;
    };
}