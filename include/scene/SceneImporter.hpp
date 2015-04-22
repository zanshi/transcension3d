//
// Created by Benjamin Wiberg on 15-04-22.
//


#pragma once

// EntityX imports
#include <entityx/entityx.h>

// AssImp imports
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

// Project-related imports
#include "components/all_components.hpp"

namespace sw {
    /*
     * SceneImporter
     * Class to handle import of ONE .dae-file specifying a scene.
     */


    class SceneImporter {
    public:
        // Constructor. 'filename' is the full name of the COLLADA file
        SceneImporter(std::string filename);

        SceneImporter() = delete;

        ~SceneImporter() { };

        static void set_relative_path_to_scene_folder(std::string relative_path);

        // Method to "copy" the contents from the AssImp node structure into the internal
        // structure of the swag3d engine. Callable only ONCE, because reasons
        void populateInternalGraph(ex::Entity rootEntity, std::function<ex::Entity()> createEntityFunction);

    private:
        static std::string relative_path_to_scene_folder_;

        // private member variables
        const aiScene *scene;
        bool has_populated_internal_graph_;

        // private member function to create new entity
        std::function<ex::Entity()> createEntity;


        // Method to "look at" a node in the tree graph returned by the Assimp importer
        // The method transfers all the node's data (including meshes etc.) to swag3d's internal structure
        // Start by calling processAssimpNode with the root node as argument
        void processAssimpNode(const aiNode *node,
                               const aiScene *scene,
                               unsigned int current_depth,
                               Dim dim_parent,
                               ex::Entity parent);
    };
}