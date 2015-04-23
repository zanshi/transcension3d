//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include <regex>

#include "glm/glm.hpp"

#include "entityx/entityx.h"
#include "entityx/deps/Dependencies.h"

#include "components/TransformComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/DimensionComponent.hpp"

#include "systems/MovementSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/DebugSystem.hpp"

// Assimp
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include "scene/SceneImporter.hpp"

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

            initScene();

            /* object name parsing tests
             *
            std::string obj_dim1 = "obj_dim1";
            std::string obj_cube_dim2 = "obj_cube_dim2";
            std::string obj_both = "obj";

            std::cout << "Object with name '" << obj_dim1 << "' is classified as dimension " << getDimensionOfNodeName(obj_dim1.c_str()) << std::endl;
            std::cout << "Object with name '" << obj_cube_dim2 << "' is classified as dimension " << getDimensionOfNodeName(obj_cube_dim2.c_str()) << std::endl;
            std::cout << "Object with name '" << obj_both << "' is classified as dimension " << getDimensionOfNodeName(obj_both.c_str()) << std::endl;
             */
        }

        void update(ex::TimeDelta dt) {
            systems.update<MovementSystem>(dt);
            systems.update<RenderSystem>(dt);
            systems.update<DebugSystem>(dt);
        }

        void initScene() {
            std::cout << "COLLADA file located in the /res folder:" << std::endl;
            std::string input;
            std::cin >> input;

            const std::string filename = input;

            // SceneImporter shell
            SceneImporter::relative_path_to_scene_folder_ = "../res/";
            SceneImporter sceneImporter(filename);

            /* The coast is clear, now we can start loading the scene */
            // Create two root objects and initialize them
            ex::Entity root = entities.create();
            initSceneGraphRoot(root);

            sceneImporter.populateInternalGraph(root, [&](){ return entities.create(); } );

        }

        // Setup function to initiate the RenderSystem with a root node
        void initSceneGraphRoot(ex::Entity root) {
            std::shared_ptr<RenderSystem> renderSystem = systems.system<RenderSystem> ();
            root_ = root;

            // The root entity should have a GraphNodeComponent, whose parent is an "empty" entity
            ex::Entity empty = entities.create();
            root_.assign<GraphNodeComponent>(empty, root_);

            // Initiate the root entity TransformComponent with an identity matrix
            root_.assign<TransformComponent>();

            // The root should be in both dimensions, for practical reasons
            root_.assign<DimensionComponent>(Dim::DIMENSION_BOTH);

            // Set the root entity to be the root of the RenderSystem i.e. where the rendering starts in the tree
            renderSystem->setRootEntity(root_);

            // Save the roots in the Application instance
            current_dim_ = Dim::DIMENSION_ONE;
        }

    private:
        ex::Entity root_;
        unsigned int current_dim_;

    };
}