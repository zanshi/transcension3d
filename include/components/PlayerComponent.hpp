//
// Created by Benjamin Wiberg on 15-05-06.
//


#pragma once

namespace sw {
    struct PlayerComponent {
        PlayerComponent(float pitch = 0.0f, float yaw = 0.0f) : pitch_(pitch) { }

        bool is_on_ground_ = false;
        float pitch_, yaw_;
    };
}