//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include <entityx/entityx.h>

#include "components/RenderComponent.hpp"
#include "components/DimensionComponent.hpp"

#include "events/RenderEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/ViewChangedEvent.hpp"

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
                : debug_ostream_(debug_stream) {
            movement_events_.reserve(1);
            view_events_.reserve(1);
        };

        void configure(ex::EventManager &events) override {
            events.subscribe<RenderEvent>(*this);
            events.subscribe<MovementEvent>(*this);
            events.subscribe<ViewChangedEvent>(*this);
        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
            /*
            debugMovementEvents();
            debugViewChangedEvents();
            debugRenderEvents();
             */
        }

        void receive(const RenderEvent &render_event) {
            // defer all debugging until DebugSystem::update(), because events are immutable and holy trinities
            //* entities_to_debug_.push_back(render_event.rendered_entity_);

            render_events_.push_back(render_event);

        }
        void debugRenderEvents() {
            for (RenderEvent event : render_events_) {
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

            render_events_.clear();
        }

        void receive(const MovementEvent &movement_event) {
            movement_events_.push_back(movement_event);
        }
        void debugMovementEvents() {
            for (MovementEvent ev : movement_events_) {
                debug_ostream_ << "Input: Movement:" << std::endl
                               << "  Right axis: " << ev.right_ << ", Forward axis: " << ev.forward_ << std::endl
                               << std::endl;
            }

            movement_events_.clear();
        }

        void receive(const ViewChangedEvent &view_changed_event) {
            view_events_.push_back(view_changed_event);
        }
        void debugViewChangedEvents() {
            for (ViewChangedEvent ev : view_events_) {
                debug_ostream_ << "Input: Mouse movement:" << std::endl
                << "  Delta Right axis: " << ev.delta_right_ << ", Delta Up axis: " << ev.delta_up_ << std::endl
                << std::endl;
            }

            view_events_.clear();
        }

        std::ostream &debug_ostream_;
        //* std::vector<ex::Entity> entities_to_debug_;
        std::vector<RenderEvent> render_events_;
        std::vector<MovementEvent> movement_events_;
        std::vector<ViewChangedEvent> view_events_;
    };
}