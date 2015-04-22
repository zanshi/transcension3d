//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include <entityx/entityx.h>

#include "components/RenderComponent.hpp"
#include "components/DimensionComponent.hpp"
#include "events/RenderEvent.hpp"

#include <ostream>
#include <vector>

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

        void configure(ex::EventManager &events) override {
            events.subscribe<RenderEvent>(*this);
        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            for (RenderEvent event : events_to_debug_) {
                ex::Entity entity = event.rendered_entity_;

                ex::ComponentHandle<DimensionComponent> dim = entity.component<DimensionComponent>();

                size_t num_indrag = event.depth_ * 3;
                std::string indrag(num_indrag, ' ');

                debug_ostream_ << indrag << "* Dimension: " << dim->dimension_ << ", Rendered entity with index=" << entity.id().index()
                << ", unique id=" << entity.id().id() << std::endl;

                // retrieve the render component data and log
                auto render = entity.component<RenderComponent>();

                if (render) {
                    debug_ostream_ << indrag << "  RenderComponent message='" << render->debug_message_ << "'"
                    << std::endl;
                }
            }

            events_to_debug_.clear();
        }

        void receive(const RenderEvent &render_event) {
            // defer all debugging until DebugSystem::update(), because events are immutable and holy trinities
            //* entities_to_debug_.push_back(render_event.rendered_entity_);

            events_to_debug_.push_back(render_event);

        }

        std::ostream &debug_ostream_;
        //* std::vector<ex::Entity> entities_to_debug_;
        std::vector<RenderEvent> events_to_debug_;
    };
}