//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include <entityx/entityx.h>
#include "glm/glm.hpp"

#include "events/RenderEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/ViewChangedEvent.hpp"


namespace ex = entityx;

namespace sw {


    class DebugSystem : public ex::System<DebugSystem>, public ex::Receiver<DebugSystem> {
    public:
        DebugSystem(std::ostream &debug_stream);

        void configure(ex::EventManager &events) override;

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt);

        void receive(const RenderEvent &render_event);

        void debugRenderEvents();

        void receive(const MovementEvent &movement_event);

        void debugMovementEvents();

        void receive(const ViewChangedEvent &view_changed_event);

        void debugViewChangedEvents();

        std::ostream &debug_ostream_;
        //* std::vector<ex::Entity> entities_to_debug_;
        std::vector<RenderEvent> render_events_;
        std::vector<MovementEvent> movement_events_;
        std::vector<ViewChangedEvent> view_events_;
    };
}