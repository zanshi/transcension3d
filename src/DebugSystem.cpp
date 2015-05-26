//
// Created by Niclas Olmenius on 2015-05-26.
//

#include "systems/DebugSystem.hpp"


#include "components/DimensionComponent.hpp"


namespace ex = entityx;

namespace sw {

    std::ostream &operator<<(std::ostream &os, const glm::vec3 v) {
        os << "{" << v[0] << ", " << v[1] << ", " << v[2] << "}";
        return os;
    }

    sw::DebugSystem::DebugSystem(std::ostream &debug_stream)
            : debug_ostream_(debug_stream) {
        movement_events_.reserve(1);
        view_events_.reserve(1);
    };


    void sw::DebugSystem::configure(ex::EventManager &events) {
        events.subscribe<RenderEvent>(*this);
        events.subscribe<MovementEvent>(*this);
        events.subscribe<ViewChangedEvent>(*this);
    }

    void sw::DebugSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
        /*
    debugMovementEvents();
    debugViewChangedEvents();
    debugRenderEvents();
     */
    }


    void sw::DebugSystem::receive(const RenderEvent &render_event) {
        // defer all debugging until DebugSystem::update(), because events are immutable and holy trinities
        //* entities_to_debug_.push_back(render_event.rendered_entity_);

        render_events_.push_back(render_event);

    }

    void sw::DebugSystem::debugRenderEvents() {
        for (sw::RenderEvent event : render_events_) {
            ex::Entity entity = event.rendered_entity_;

            ex::ComponentHandle <DimensionComponent> dim = entity.component<DimensionComponent>();

            size_t num_indrag = event.depth_ * 3;
            std::string indrag(num_indrag, ' ');

            debug_ostream_ << indrag << "* Dimension: " << dim->dimension_ << ", Rendered entity with index=" <<
            entity.id().index()
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

    void sw::DebugSystem::receive(const MovementEvent &movement_event) {
        movement_events_.push_back(movement_event);
    }

    void sw::DebugSystem::debugMovementEvents() {
        for (MovementEvent ev : movement_events_) {
            debug_ostream_ << "Input: Movement:" << std::endl
            << "  Right axis: " << ev.right_ << ", Forward axis: " << ev.forward_ << std::endl
            << std::endl;
        }

        movement_events_.clear();
    }

    void sw::DebugSystem::receive(const ViewChangedEvent &view_changed_event) {
        view_events_.push_back(view_changed_event);
    }

    void sw::DebugSystem::debugViewChangedEvents() {
        for (ViewChangedEvent ev : view_events_) {
            debug_ostream_ << "Input: Mouse movement:" << std::endl
            << "  Delta Right axis: " << ev.delta_right_ << ", Delta Up axis: " << ev.delta_up_ << std::endl
            << std::endl;
        }

        view_events_.clear();
    }


}