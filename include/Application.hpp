//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "glm/glm.hpp"

#include "entityx/entityx.h"
#include "entityx/deps/Dependencies.h"

#include "components/components.hpp"

#include "systems/MovementSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/DebugSystem.hpp"

namespace ex = entityx;

namespace sw {

    class Application : public ex::EntityX {
    public:
        explicit Application() {
            systems.add<MovementSystem>();
            systems.add<RenderSystem>();
            systems.add<DebugSystem>(std::cout);

            // An object can't move unless it has a position, D'UH
            //systems.add<ex::deps::Dependency<MovementComponent, BodyComponent>>();
            systems.configure();

            init();
        }

        void update(ex::TimeDelta dt) {
            systems.update<MovementSystem>(dt);
            systems.update<RenderSystem>(dt);
            systems.update<DebugSystem>(dt);
        }

        // init with some uninteresting, crappy entities
        void init() {
            /** Entity 1 **/
            ex::Entity entity1 = entities.create();

            // entity.assign returns a handle of the specified type,
            // can be useful for "remembering" values when initializing, like so:
            //ex::ComponentHandle<BodyComponent> bc1 =
            entity1.assign<BodyComponent>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            entity1.assign<RenderComponent>("Entity 1: Benjamin");

            /** Entity 2 **/
            ex::Entity entity2 = entities.create();
            entity2.assign<BodyComponent>(glm::vec3(10.0f, 0.0f, -4.3f), glm::vec3(4.0f, 9.0f, 0.0f));
            entity2.assign<MovementComponent>(glm::vec3(2.0f, 0.0f, 0.2f), 100.0f);
            entity2.assign<RenderComponent>("Entity 2: Niclas");

            /** Entity 3 **/
            ex::Entity entity3 = entities.create();
            auto render = entity3.assign<RenderComponent>("Entity 3: Obama");
        }
    };

}