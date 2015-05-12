//
// Created by Benjamin Wiberg on 2015-04-13.
//


#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <entityx/entityx.h>

namespace ex = entityx;

namespace sw {

    struct TransformComponent : public ex::Component<TransformComponent> {
        // Default position and rotation is {0}
        TransformComponent(glm::vec3 position = {0.0f, 0.0f, 0.0f},
                           glm::vec3 rotation = {0.0f, 0.0f, 0.0f})
                : position_(position), rotation_(rotation), orientation_(glm::quat(rotation)) {
            update_local_transform();
            is_dirty_ = true;
        };

        TransformComponent(glm::mat4 local_transform)
                : local_(local_transform) {
            glm::vec3 temp3;
            glm::vec4 temp4;
            glm::decompose(local_, temp3, orientation_, position_, temp3, temp4);

            is_dirty_ = true;
        }

        /** Public member variables for convenience **/
        glm::vec3 position_;
        glm::quat orientation_;
        glm::mat4 local_;
        glm::mat4 cached_world_;


        // TODO: remove this
        glm::vec3 rotation_; // Contains: {rotX, rotY, rotZ}

        // Set this to true if position or rotation has been updated
        bool is_dirty_;

        void update_local_transform() {
            // TODO : Test this code snippet
            local_ = glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(orientation_);
        }
    };
}