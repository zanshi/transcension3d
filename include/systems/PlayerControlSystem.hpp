//
// Created by Cristoffer Särevall on 2015-05-06.
//

#pragma once

#include "entityx/entityx.h"

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/ViewEvent.hpp"


namespace ex = entityx;

namespace sw {
    class PlayerControlSystem : public ex::System<PlayerControlSystem>, public ex::Receiver<PlayerControlSystem> {

        public:

        void configure(ex::EventManager &events) override {
            events.subscribe<JumpEvent>(*this);
            events.subscribe<MovementEvent>(*this);
            events.subscribe<ViewEvent>(*this);
        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
        }

        void receive(const JumpEvent &jump) {

        }
        void receive(const MovementEvent &move) {

        }
        void receive(const ViewEvent &view) {

        }
    };
}
