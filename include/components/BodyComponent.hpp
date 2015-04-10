//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "glm/glm.hpp"

namespace sw {

    struct BodyComponent {
        BodyComponent(const glm::vec3 position, const glm::vec3 rotation)
                : position_(position), rotation_(rotation) { };

        glm::vec3 position_;
        glm::vec3 rotation_;
    };

}