//
// Created by Benjamin Wiberg on 2015-04-13.
//


#pragma once

#include <entityx/config.h>
#include "components/DimensionComponent.hpp"

namespace ex = entityx;

namespace sw {
    const Dim STARTING_DIMENSION = Dim::DIMENSION_ONE;

    const ex::TimeDelta TIME_STEP = 0.01;

    enum Collisiontypes {
        COL_NOTHING = 1 << 0,
        COL_STATIC = 1 << 1,
        COL_DYNAMIC = 1 << 2,
        COL_PLAYER = 1 << 3
    };

    const int WINDOW_WIDTH = 1280,
            WINDOW_HEIGHT = 960;

    const float FOVY_DEGREES = 65.f;

    enum States {
        STATE_STANDING,
        STATE_AIRBOURNE
    };
}