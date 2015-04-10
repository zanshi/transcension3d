//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "glm/glm.hpp"

namespace sw {

    struct MovementComponent {
        MovementComponent(const glm::vec3 velocity, const float mass)
                : velocity_(velocity), mass_(mass) { };

        glm::vec3 velocity_;
        float mass_;
    };

}