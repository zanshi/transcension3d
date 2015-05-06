//
// Created by Johan Nordin on 28/04/15.
//

#pragma once

#include <iosfwd>
#include <string>

namespace sw {

    struct Color {
        Color(glm::vec3 ambient = {} , glm::vec3 diffuse = {}, glm::vec3 specular = {} )
                : ambient_(ambient), diffuse_(diffuse), specular_(specular) { }

        glm::vec3 ambient_, diffuse_, specular_;

    };

    struct LightComponent : public ex::Component<LightComponent> {
        LightComponent(Color color = {}, glm::vec3 position = {})
                : color_(color), position_(position) { }

        Color color_;
        string name_;
        glm::vec3 position_;


    };


}
