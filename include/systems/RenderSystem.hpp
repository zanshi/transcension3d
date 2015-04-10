//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "entityx/entityx.h"

#include "components/BodyComponent.hpp"
#include "components/RenderComponent.hpp"

#include "events/RenderEvent.hpp"

namespace ex = entityx;

namespace sw {

/** Placeholder rendering system that processes entities with the following components
 *      - BodyComponent
 *      - RenderComponent
 *
 *  It logs the BodyComponent's position and rotation vectors as well as the RenderComponent's debug string
 *  **/

    struct RenderSystem : public ex::System<RenderSystem> {
        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            ex::ComponentHandle <BodyComponent> body;
            ex::ComponentHandle <RenderComponent> render;

            for (ex::Entity entity : es.entities_with_components(body, render)) {
                // RENDER -> represented by emitting a RenderEvent, TODO: make it ACTUALLY render something...
                events.emit<RenderEvent>(entity);
            }
        }
    };
}