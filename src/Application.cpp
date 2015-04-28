//
// Created by Benjamin Wiberg on 15-04-23.
//

#include "Application.hpp"

// Assimp
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include "scene/SceneImporter.hpp"

#include "components/all_components.hpp"

#include "systems/MovementSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/DebugSystem.hpp"
#include "systems/PhysicsSystem.hpp"

sw::Application::Application() {
    systems.add<MovementSystem>();
    systems.add<RenderSystem>(events);
    systems.add<DebugSystem>(std::cout);
    systems.add<PhysicsSystem>();

    // An object can't move unless it has a position, D'UH
    //systems.add<ex::deps::Dependency<MovementComponent, BodyComponent>>();
    systems.configure();
}

void sw::Application::update(ex::TimeDelta dt) {
    systems.update<MovementSystem>(dt);
    systems.update<RenderSystem>(dt);
    systems.update<DebugSystem>(dt);
}

void sw::Application::initScene() {
    // TODO: handle scene loading more gracefully
    std::cout << "COLLADA file located in the /res folder:" << std::endl;
    std::string input;
    std::cin >> input;

    //input = "a1.dae"; // test file
    const std::string filename = input;

    // SceneImporter shell
    SceneImporter::relative_path_to_scene_folder_ = "../res/";
    SceneImporter sceneImporter(filename);

    /* The coast is clear, now we can start loading the scene */
    // Create two root objects and initialize them
    ex::Entity root = entities.create();
    initSceneGraphRoot(root);

    sceneImporter.populateInternalGraph(root, [&]() { return entities.create(); });

    auto renderSystem = systems.system < RenderSystem > ();

    // Camera stuff
    glm::vec3 world_cameraLookAt = sceneImporter.getCameraLookAt();
    glm::vec3 world_cameraPosition = sceneImporter.getCameraPosition();
    glm::mat4 cameraProjection = sceneImporter.getCameraProjectionMatrix();
    renderSystem->setCamera(world_cameraPosition, world_cameraLookAt);
    renderSystem->setProjectionMatrix(cameraProjection);
}

// Setup function to initiate the RenderSystem with a root node
void sw::Application::initSceneGraphRoot(ex::Entity root) {
    auto renderSystem = systems.system < RenderSystem > ();
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
