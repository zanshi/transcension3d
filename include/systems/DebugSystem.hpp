//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include <entityx/entityx.h>

#include "components/BodyComponent.hpp"
#include "components/RenderComponent.hpp"
#include "events/RenderEvent.hpp"

#include <ostream>

#include "glm/glm.hpp"

namespace ex = entityx;

namespace sw {
    std::ostream &operator<<(std::ostream &os, const glm::vec3 v) {
        os << "{" << v[0] << ", " << v[1] << ", " << v[2] << "}";
        return os;
    }

    class DebugSystem : public ex::System<DebugSystem>, public ex::Receiver<DebugSystem> {
    public:
        DebugSystem(std::ostream &debug_stream)
                : debug_ostream_(debug_stream) { };

        void configure(ex::EventManager &event_manager) override {
            event_manager.subscribe<RenderEvent>(*this);
        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) { }

        void receive(const RenderEvent &render_event) {
            debug_ostream_ << "Rendered entity with index=" << render_event.rendered_entity_.id().index()
            << ", unique id=" << render_event.rendered_entity_.id().id() << std::endl;

            auto body = render_event.rendered_entity_.component<BodyComponent>();

            if (body) {
                debug_ostream_ << "  BodyComponent: " << "Position=" << body->position_
                << ", Rotation=" << body->rotation_ << std::endl
                << std::endl;
            }

            auto render = render_event.rendered_entity_.component<RenderComponent>();

            if (render) {
                debug_ostream_ << "ZOMG" << std::endl;
            }
        }

        std::ostream &debug_ostream_;
    };
}