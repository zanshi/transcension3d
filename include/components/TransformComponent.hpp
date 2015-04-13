//
// Created by Benjamin Wiberg on 2015-04-13.
//


#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

namespace sw {
    class TransformComponent {
    public:
        // Default position and rotation is {0}
        TransformComponent(glm::vec3 position = {0.0f, 0.0f, 0.0f},
                           glm::vec3 rotation = {0.0f, 0.0f, 0.0f})
                : position_(position), rotation_(rotation) { };

        /** Public member variables for convenience **/
        glm::vec3 position_;
        glm::vec3 rotation_;
        glm::mat4 local_;
        glm::mat4 cached_world_;

        // Set this to true if position or rotation has been updated
        bool is_dirty;
    };
}