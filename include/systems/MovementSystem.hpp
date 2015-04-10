//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "entityx/entityx.h"

#include "components/BodyComponent.hpp"
#include "components/MovementComponent.hpp"

namespace ex = entityx;

namespace sw {

    /** Simple movement system that processes entities with the following components
     *      - BodyComponent
     *      - MovementComponent
     *
     *  It updates the BodyComponent's position vector using the MovementComponent's velocity vector using Euler integration
     *  **/
    struct MovementSystem : public ex::System<MovementSystem> {
        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            ex::ComponentHandle <BodyComponent> body;
            ex::ComponentHandle <MovementComponent> movement;

            for (ex::Entity entity : es.entities_with_components(body, movement)) {
                //Simple Euler integration of position
                body->position_ += movement->velocity_ * static_cast<float>(dt);

                /* Debugging
                std::cout << "MovementSystem:" << std::endl
                << "  Operating on entity with id=" << entity.id().id() << std::endl
                << "    Rotation: " << std::endl
                << std::endl;
                 */
            }
        }
    };

}