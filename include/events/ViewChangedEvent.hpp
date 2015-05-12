//
// Created by cristoffer on 2015-05-05.
//

#pragma once

namespace sw {
    struct ViewChangedEvent {
        ViewChangedEvent(int x, int y) : delta_up_(y), delta_right_(x) {}

        float delta_up_, delta_right_;
    };
}