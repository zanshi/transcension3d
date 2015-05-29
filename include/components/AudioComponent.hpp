//
// Created by Johan Nordin on 26/05/15.
//

#pragma once

namespace ex = entityx;

namespace sw {

    struct AudioComponent : public ex::Component<AudioComponent> {

        AudioComponent(std::string name = "", float shininess = 1.0f)
                : name_(name), shininess_(shininess) { }


        std::string name_;
        float shininess_;

    };
}