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
            systems.add<RenderSystem>(events);
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
            root.assign<GraphNodeComponent>(entities.create(), root);
            // The root entity should have the identity matrix as its Transform
            root.assign<TransformComponent>();
            root.assign<RenderComponent>("root");
            // Set the root entity to be the root of the RenderSystem i.e. where the rendering starts in the tree
            std::shared_ptr<RenderSystem> renderSystem = systems.system<RenderSystem>();
            renderSystem->setRootEntity(root);

            /** Create "real" entities to be shown in the world **/
            // 1
            ex::Entity e1 = entities.create();
            e1.assign<GraphNodeComponent>(root, e1);
            e1.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e1.assign<RenderComponent>("e1");
            // 2
            ex::Entity e2 = entities.create();
            e2.assign<GraphNodeComponent>(root, e2);
            e2.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e2.assign<RenderComponent>("e2");
            // 3
            ex::Entity e3 = entities.create();
            e3.assign<GraphNodeComponent>(e1, e3);
            e3.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e3.assign<RenderComponent>("e3");
            // 4
            ex::Entity e4 = entities.create();
            e4.assign<GraphNodeComponent>(e1, e4);
            e4.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e4.assign<RenderComponent>("e4");
            // 5
            ex::Entity e5 = entities.create();
            e5.assign<GraphNodeComponent>(e4, e5);
            e5.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e5.assign<RenderComponent>("e5");
            // 6
            ex::Entity e6 = entities.create();
            e6.assign<GraphNodeComponent>(e2, e6);
            e6.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e6.assign<RenderComponent>("e6");
            // 7
            ex::Entity e7 = entities.create();
            e7.assign<GraphNodeComponent>(e6, e7);
            e7.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e7.assign<RenderComponent>("e7");
            // 8
            ex::Entity e8 = entities.create();
            e8.assign<GraphNodeComponent>(e6, e8);
            e8.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e8.assign<RenderComponent>("e8");
            // 9
            ex::Entity e9 = entities.create();
            e9.assign<GraphNodeComponent>(e2, e9);
            e9.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e9.assign<RenderComponent>("e9");
            // 10
            ex::Entity e10 = entities.create();
            e10.assign<GraphNodeComponent>(e2, e10);
            e10.assign<TransformComponent>(vec3(1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.3f));
            e10.assign<RenderComponent>("e10");
        }
    };
}