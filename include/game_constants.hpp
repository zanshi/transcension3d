//
// Created by Benjamin Wiberg on 2015-04-13.
//


#pragma once

#include <entityx/config.h>

namespace ex = entityx;

namespace sw {
    const ex::TimeDelta TIME_STEP = 0.01;

    enum Collisiontypes {
        COL_NOTHING = 1 << 0,
        COL_STATIC = 1 << 1,
        COL_DYNAMIC = 1 << 2,
        COL_PLAYER = 1 << 3
    };


}