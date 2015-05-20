//
// Created by Benjamin Wiberg on 15-05-12.
//


#pragma once

namespace sw {
    struct DimensionChangeInProgressEvent {
        DimensionChangeInProgressEvent(float completion_factor)
            : completion_factor_(completion_factor) {}

        // How much of the dimChange is completed:
        // 0.0 represents just started
        // 0.5 represents the point when the dimChange occurs
        // 1.0 represents fully completed
        float completion_factor_;
    };
}