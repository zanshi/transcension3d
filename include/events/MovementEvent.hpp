//
// Created by cristoffer on 2015-05-05.
//

#pragma once

namespace sw {
    struct MovementEvent {
        MovementEvent(float x, float y, bool is_sprinting = false) : right_(x) , forward_(y), is_sprinting_(is_sprinting) {}

        float forward_, right_;
        bool is_sprinting_;
    };
}