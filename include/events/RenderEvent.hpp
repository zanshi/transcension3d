//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include <entityx/entityx.h>

#include "components/RenderComponent.hpp"

namespace ex = entityx;

namespace sw {

    struct RenderEvent {
        RenderEvent(ex::Entity rendered_entity, unsigned int depth)
                : rendered_entity_(rendered_entity), depth_(depth) { };

        ex::Entity rendered_entity_;
        unsigned int depth_;
    };
}