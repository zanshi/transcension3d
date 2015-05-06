//
// Created by Johan Nordin on 28/04/15.
//

#pragma once

#include <iosfwd>
#include <string>

namespace sw {


    struct LightComponent : public ex::Component<LightComponent> {
        LightComponent(Color color = {}, glm::vec3 position = {})
                : color_(color), position_(position) { }

        Color color_;
        std::string name_;
        glm::vec3 position_;


    };


}
