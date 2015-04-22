//
// Created by Benjamin Wiberg on 15-04-22.
//

#pragma once

#include "scene/SceneImporter.hpp"

#include <functional>

namespace sw {
    SceneImporter::SceneImporter(std::string filename) {
        Assimp::Importer importer;
        scene = importer.ReadFile(filename.c_str(), 0);
        if (scene == nullptr) {
            std::cout << "The file could not be loaded. Exiting." << std::endl;
            assert(scene);
        }

        has_populated_internal_graph_ = false;
    }

    void SceneImporter::populateInternalGraph(ex::Entity rootEntity, std::function<ex::Entity()> createEntityFunction) {
        // Don't populate the application if it has already been done
        if (has_populated_internal_graph_)
            return;
        else
            has_populated_internal_graph_ = true;

        createEntity = createEntityFunction;

        // Populate the graph
        processAssimpNode(scene->mRootNode, scene, 0, Dim::DIMENSION_BOTH, rootEntity);

    }

    void SceneImporter::processAssimpNode(const aiNode *node, const aiScene *scene, unsigned int current_depth,
                                          Dim dim_parent, ex::Entity parent) {
        /*
        assert(dim_parent == Dim::DIMENSION_BOTH || dim_parent == Dim::DIMENSION_ONE || dim_parent == Dim::DIMENSION_TWO
                                                                                        &&
                                                                                        "Something strange happened with the assimp processing.");
        */

        if ( !(dim_parent == Dim::DIMENSION_BOTH || dim_parent == Dim::DIMENSION_ONE || dim_parent == Dim::DIMENSION_TWO) ) {
            dim_parent = Dim::DIMENSION_BOTH;
        }

        // If parent belongs to either dimension 1 or 2 -> all its children belongs to the same dimension
        // If parent belongs to both dimensions -> determine which dimension(-s) its children belongs to
        Dim dim_current;
        if (dim_parent == Dim::DIMENSION_BOTH) {
            // Determine which dimension the current node and its children belongs to (can be both)
            dim_current = getDimensionOfNodeName(node->mName.C_Str());
        } else {
            // Parent belongs to either dimension 1 or 2 -> this node does as well
            dim_current = dim_parent;
        }

        /* Add the current node to its parent */
        ex::Entity current_entity = createEntity();
        current_entity.assign<TransformComponent>();
        current_entity.assign<GraphNodeComponent>(parent, current_entity);
        current_entity.assign<DimensionComponent>(dim_current);
        current_entity.assign<RenderComponent>( std::string(node->mName.C_Str()) );

        // Recursively process all its children nodes
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            if (*(node->mChildren + i))
                processAssimpNode(*(node->mChildren + i), scene, current_depth + 1, dim_current, current_entity);
        }
    }

    void SceneImporter::setRelativePathToSceneFolder(std::string relative_path) {
        relative_path_to_scene_folder_ = relative_path;
    }
}