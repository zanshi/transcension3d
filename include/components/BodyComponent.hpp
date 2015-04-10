//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "glm/glm.hpp"

namespace sw {

    struct BodyComponent {
        BodyComponent(glm::vec3 position = {0.0f, 0.0f, 0.0f}, glm::vec3 rotation = {0.0f, 0.0f, 0.0f})
                : position_(position), rotation_(rotation) { };

        glm::vec3 position_;
        glm::vec3 rotation_;
    };

}