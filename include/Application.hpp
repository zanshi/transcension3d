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

            const std::string filename = "../res/" + input;

            Assimp::Importer importer;
            // Import the scene in COLLADA format
            const aiScene *scene = importer.ReadFile(filename.c_str(), 0);
            if (scene == nullptr) {
                std::cout << "The file could not be loaded." << std::endl;
                assert(scene);
            }

            /* The coast is clear, now we can start loading the scene */
            // Create two root objects and initialize them
            ex::Entity root = entities.create();
            initSceneGraphRoot(root);

            const aiNode *scene_root = scene->mRootNode;

            // Begin the tree traversal
            processAssimpNode(scene_root, scene, 0, DIMENSION_BOTH, root);


        }

        // Setup function to initiate the RenderSystem with a root node
        void initSceneGraphRoot(ex::Entity root) {
            std::shared_ptr<RenderSystem> renderSystem = systems.system < RenderSystem > ();
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

        // TODO: Move node classification out of here

        const std::regex REGEX_MATCH_DIMENSION_ONE = std::regex("\\S+_dim1");
        const std::regex REGEX_MATCH_DIMENSION_TWO = std::regex("\\S+_dim2");


        const Dim getDimensionOfNodeName(const char *str) {
            if (std::regex_match(str, REGEX_MATCH_DIMENSION_ONE))
                return DIMENSION_ONE;

            if (std::regex_match(str, REGEX_MATCH_DIMENSION_TWO))
                return DIMENSION_TWO;

            return DIMENSION_BOTH;
        }

        // Method to "look at" a node in the tree graph returned by the Assimp importer
        // The method transfers all the node's data (including meshes etc.) to swag3d's internal structure
        // Start by calling processAssimpNode with the root node as argument
        void processAssimpNode(const aiNode *node, const aiScene *scene, unsigned int current_depth,
                               Dim dim_parent, ex::Entity parent) {
            assert(dim_parent == Dim::DIMENSION_BOTH || dim_parent == Dim::DIMENSION_ONE || dim_parent == Dim::DIMENSION_TWO
                                                                                  &&
                                                                                  "Something strange happened with the assimp processing.");

            // If parent belongs to either dimension 1 or 2 -> all its children belongs to the same dimension
            // If parent belongs to both dimensions -> determine which dimension(-s) its children belongs to
            Dim dim_node;
            if (dim_parent == DIMENSION_BOTH) {
                // Determine which dimension the current node and its children belongs to (can be both)
                dim_node = getDimensionOfNodeName(node->mName.C_Str());
            } else {
                // Parent belongs to either dimension 1 or 2 -> this node does as well
                dim_node = dim_parent;
            }

            /* Add the current node to its parent */
            ex::Entity current_entity = entities.create();
            current_entity.assign<TransformComponent>();
            current_entity.assign<GraphNodeComponent>(parent, current_entity);
            current_entity.assign<DimensionComponent>(dim_node);
            current_entity.assign<RenderComponent>( std::string(node->mName.C_Str()) );

            // Recursively process all its children nodes
            for (unsigned int i = 0; i < node->mNumChildren; ++i) {
                if (*(node->mChildren + i))
                    processAssimpNode(*(node->mChildren + i), scene, current_depth + 1, dim_node, current_entity);
            }
        }

        /*** UNUSED ***/

        /*
        // Assimp loading test: load test.dae (COLLADA format)
        void initAssimp() {
            std::cout << "COLLADA file located in the /res folder:" << std::endl;
            std::string input;
            std::cin >> input;

            const std::string filename = "../res/" + input;

            Assimp::Importer importer;
            // Import the scene in COLLADA format
            const aiScene *scene = importer.ReadFile(filename.c_str(), 0);
            if (scene == nullptr) {
                std::cout << "The file could not be loaded." << std::endl;
                assert(scene);
            }

            // Traverse the tree graph
            const aiNode *root = scene->mRootNode;

            std::cout << "Traversing the tree '" << filename << "', starting with the root node." << std::endl
            << "===================" << std::endl;

            traverseNode(scene, root, 0);
        }
         */

        /*
        void traverseNode(const aiScene *scene, const aiNode *current, unsigned int depth) {
            using namespace std;

            const unsigned int indrag_c = 3;
            const size_t indrag = depth * indrag_c;
            string indrag_s(indrag, ' ');

            cout << indrag_s
            << "* Current node name: '" << current->mName.C_Str() << "'." << endl;


            // Display the details regarding the 'current' node
            if (current->mNumMeshes != 0) {
                cout << indrag_s
                << "Meshes: " << current->mNumMeshes << endl;

                for (int i = 0; i < current->mNumMeshes; ++i) {
                    const aiMesh *mesh = *(scene->mMeshes + i);

                    // Display info about the mesh
                    cout << indrag_s
                    << "Mesh " << i << ": mNumVertices=" << mesh->mNumVertices << endl;
                }
            }

            ++depth;

            for (int i = 0; i < current->mNumChildren; ++i) {
                const aiNode *child = *(current->mChildren + i);
                if (child != nullptr) {
                    traverseNode(scene, child, depth);
                }
            }
        }
         */

    };
}