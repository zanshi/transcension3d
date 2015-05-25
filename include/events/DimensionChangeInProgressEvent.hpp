//
// Created by Benjamin Wiberg on 15-05-12.
//


#pragma once
#include "components/DimensionComponent.hpp"

namespace sw {
    struct DimensionChangeInProgressEvent {
        DimensionChangeInProgressEvent(float completion_factor, Dim dim_from, int dimChangeNum)
            : completion_factor_(completion_factor), dim_from_(dim_from), NUM_(dimChangeNum) {}

        // How much of the dimChange is completed:
        // 0.0 represents just started
        // 0.5 represents the point when the dimChange occurs
        // 1.0 represents fully completed
        Dim dim_from_;
        float completion_factor_;
        const int NUM_;
    };
}