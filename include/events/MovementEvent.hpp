//
// Created by cristoffer on 2015-05-05.
//

#pragma once

namespace sw {
    struct MovementEvent {
        MovementEvent(int x, int y) : x(x) , y(y) {}

        int x, y;
    };
}