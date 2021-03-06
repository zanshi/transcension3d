//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

namespace ex = entityx;

namespace sw {

    struct RenderComponent : public ex::Component<RenderComponent> {
        RenderComponent(std::string debug_message = "Default message")
                : debug_message_(debug_message) { };

        std::string debug_message_;
    };

}