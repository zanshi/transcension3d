//
// Created by cristoffer on 2015-05-05.
//

#pragma once

namespace sw {
    struct MovementEvent {
        MovementEvent(int x, int y) : right_(x) , forward_(y) {}

        int forward_, right_;
    };
}