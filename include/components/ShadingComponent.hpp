//
// Created by Niclas Olmenius on 15-04-24.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/detail/type_vec.hpp>


namespace sw {

    struct Color {
        Color(glm::vec3 ambient = {} , glm::vec3 diffuse = {}, glm::vec3 specular = {} )
                : ambient_(ambient), diffuse_(diffuse), specular_(specular) { }

        glm::vec3 ambient_, diffuse_, specular_;

    };


    struct ShadingComponent : public ex::Component<ShadingComponent> {

        ShadingComponent(Color color = {}, float shininess = 1.0f)
                : color_(color), shininess_(shininess) { }

        Color color_;
        float shininess_;

    };


}