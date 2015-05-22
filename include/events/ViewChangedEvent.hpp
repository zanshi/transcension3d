//
// Created by cristoffer on 2015-05-05.
//

#pragma once

namespace sw {
    struct ViewChangedEvent {
        ViewChangedEvent(float x, float y) : delta_up_(y), delta_right_(x) {}

        // The value in these represents the ratio of 'pixels mouse moved' / 'window width'
        // Scale accordingly! And safely...
        float delta_up_, delta_right_;
    };
}