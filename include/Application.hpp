//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "glm/glm.hpp"

#include "entityx/entityx.h"
#include "entityx/deps/Dependencies.h"

#include "components/TransformComponent.hpp"
#include "components/MovementComponent.hpp"

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
            using glm::vec3;

            /** Init a crappy root object **/
            ex::Entity root = entities.create();
            // The root entity should have a GraphNodeComponent, whose parent is an "empty" entity
            root.assign<GraphNodeComponent>(entities.create());
            // The root entity should have the identity matrix as its Transform
            root.assign<TransformComponent>();
            // Set the root entity to be the root of the RenderSystem i.e. where the rendering starts in the tree
            std::shared_ptr<RenderSystem> renderSystem = systems.system<RenderSystem>();
            renderSystem->setRootEntity(root);

            /** Entity 1 **/
            ex::Entity entity1 = entities.create();
            entity1.assign<GraphNodeComponent>(root, entity1);
            entity1.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.5f));

            // entity.assign returns a handle of the specified type,
            // can be useful for "remembering" values when initializing, like so:
            //ex::ComponentHandle<BodyComponent> bc1 =
            entity1.assign<RenderComponent>("Entity 1: Benjamin");

            /** Entity 2 **/
            ex::Entity entity2 = entities.create();
            entity2.assign<MovementComponent>(vec3(2.0f, 0.0f, 0.2f), 100.0f);
            entity2.assign<RenderComponent>("Entity 2: Niclas");

            /** Entity 3 **/
            ex::Entity entity3 = entities.create();
            auto render = entity3.assign<RenderComponent>("Entity 3: Obama");
        }
    };
}