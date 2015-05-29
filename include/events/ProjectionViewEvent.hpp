//
// Created by Niclas Olmenius on 2015-05-25.
//

#pragma once


namespace sw {
    struct ProjectionViewEvent {
        ProjectionViewEvent(glm::mat4 proj, glm::mat4 view)
                : projection_matrix_(proj), view_matrix_(view) { }

        glm::mat4 projection_matrix_;
        glm::mat4 view_matrix_;

    };
}