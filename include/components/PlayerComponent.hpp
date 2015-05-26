//
// Created by Benjamin Wiberg on 15-05-06.
//


#pragma once

namespace sw {
    struct PlayerComponent {
        PlayerComponent(float pitch = 0.0f, float yaw = 0.0f) : pitch_(pitch) { }

        int state_ = STATE_STANDING;
        float pitch_, yaw_;
        float distance_to_bottom_ = 0.0f;
    };
}