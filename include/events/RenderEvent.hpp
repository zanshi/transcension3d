//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include <entityx/entityx.h>

#include "components/RenderComponent.hpp"

namespace ex = entityx;

namespace sw {

    struct RenderEvent {
        RenderEvent(ex::Entity rendered_entity)
                : rendered_entity_(rendered_entity) { };

        ex::Entity rendered_entity_;
    };
}