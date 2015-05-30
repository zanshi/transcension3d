//
// Created by Benjamin Wiberg on 15-04-22.
//


#pragma once

// std::function for createEntity function
#include <functional>
#include <string>

#include "glm/glm.hpp"

// EntityX imports
#include <entityx/entityx.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <sys/cdefs.h>
#include <game_constants.hpp>

// AssImp imports
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include "components/DimensionComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"

namespace ex = entityx;

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

        ~SceneImporter() {
            //delete camera_node_;
            //delete p_scene;
        };

        static std::string relative_path_to_scene_folder_;

        // Method to "copy" the contents from the AssImp node structure into the internal
        // structure of the Transcension3D engine. Callable only ONCE, because reasons
        void populateInternalGraph(std::function<ex::Entity()> createEntityFunction);

        glm::mat4 getCamera();

    private:
        // private member variables
        Assimp::Importer importer;
        const aiScene *p_scene;
        bool has_populated_internal_graph_;

        const aiNode *camera_node_;

        // private member function to create new entity
        std::function<ex::Entity()> createEntity;


        // Method to "look at" a node in the tree graph returned by the Assimp importer
        // The method transfers all the node's data (including meshes etc.) to Transcension3D's internal structure
        // Start by calling processAssimpNode with the root node as argument
        void processAssimpNode(const aiNode *node,
                               unsigned int current_depth,
                               Dim dim_parent,
                               glm::mat4 parent_transform);

        void addMeshComponentToEntity(ex::Entity entity, const aiMesh *mesh);

        void addShadingComponentToEntity(entityx::Entity entity, const aiMesh *mesh);

        void addLightComponentToEntity(entityx::Entity entity, const aiLight *light);

        const aiLight *getLightWithName(const char *str);

        void addPhysicsComponentToEntity(entityx::Entity entity, float mass, short group,
                                         short mask);

        glm::vec3 buildBoundingVector(glm::mat4 world_transform, std::vector<Vertex> vertices);

        btConvexHullShape *buildCollisionShape(glm::vec3 scale, std::vector<Vertex> vertices,
                                                              std::vector<GLuint> indices, btScalar &height);


    };
}