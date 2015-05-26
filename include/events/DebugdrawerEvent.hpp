//
// Created by Johan Nordin on 25/05/15.
//

#pragma once

namespace sw {
    struct DebugdrawerEvent {
        DebugdrawerEvent(bool debugMode)
                : debugMode_(debugMode) {}

        float debugMode_;
    };
}
