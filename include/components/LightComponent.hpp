//
// Created by Johan Nordin on 28/04/15.
//

#pragma once

#include <iosfwd>
#include <string>

namespace sw {
    struct LightComponent : public ex::Component<LightComponent> {
        enum LightType {
            POINT,
            DIRECTIONAL,
            SPOT
        };

        LightComponent(Color color = {}, LightType type = POINT)
                : color_(color) { }

        Color color_;
        LightType type_;
    };


}
