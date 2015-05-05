//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "glm/glm.hpp"

namespace sw {

    struct MovementComponent : public ex::Component<MovementComponent> {
        MovementComponent(glm::vec3 velocity = {0.0f, 0.0f, 0.0f}, float mass = 1.0f)
                : velocity_(velocity), mass_(mass) { };

        glm::vec3 velocity_;
        float mass_;
    };

}