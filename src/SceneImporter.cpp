//
// Created by Benjamin Wiberg on 15-04-22.
//

#include "scene/SceneImporter.hpp"
#include <regex>
#include "game_constants.hpp"

// GL Includes
//#include <GL/glew.h>

// GLM includes
#include <glm/glm.hpp>
#include <assimp/postprocess.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>


// Bullet includes
#include "btBulletDynamicsCommon.h"

// Project-related imports
#include "components/all_components.hpp"

namespace sw {
    std::string SceneImporter::relative_path_to_scene_folder_ = "../res/";

    SceneImporter::SceneImporter(std::string filename) {
        filename = relative_path_to_scene_folder_ + filename;
        p_scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate);
        if (p_scene == nullptr) {
            std::cout << "The file could not be loaded. Exiting." << std::endl;
            assert(p_scene);
        }

        has_populated_internal_graph_ = false;
    }

    /* Mesh methods  */
    glm::vec3 aiVector_to_glmVec(const aiVector3D in_vec) {
        return glm::vec3(in_vec[0], in_vec[1], in_vec[2]);
    }

    glm::vec2 aiVector_to_glmVec(const aiVector2D in_vec) {
        return glm::vec2(in_vec[0], in_vec[1]);
    }

    glm::mat4 aiMatrix4x4_to_glmMat4(const aiMatrix4x4 aiMat) {
        glm::mat4 m;

        m[0][0] = aiMat[0][0];
        m[0][1] = aiMat[1][0];
        m[0][2] = aiMat[2][0];
        m[0][3] = aiMat[3][0];

        m[1][0] = aiMat[0][1];
        m[1][1] = aiMat[1][1];
        m[1][2] = aiMat[2][1];
        m[1][3] = aiMat[3][1];

        m[2][0] = aiMat[0][2];
        m[2][1] = aiMat[1][2];
        m[2][2] = aiMat[2][2];
        m[2][3] = aiMat[3][2];

        m[3][0] = aiMat[0][3];
        m[3][1] = aiMat[1][3];
        m[3][2] = aiMat[2][3];
        m[3][3] = aiMat[3][3];

        return m;
    }

    glm::mat4 SceneImporter::getCamera() {
        if (camera_node_ != nullptr) {
            return aiMatrix4x4_to_glmMat4(camera_node_->mTransformation);
        } else {
            return glm::mat4(1);
        }
    }

    void SceneImporter::populateInternalGraph(ex::Entity rootEntity, std::function<ex::Entity()> createEntityFunction) {
        // Don't populate the application if it has already been done
        if (has_populated_internal_graph_)
            return;
        else
            has_populated_internal_graph_ = true;

        createEntity = createEntityFunction;

        // Populate the graph
        processAssimpNode(p_scene->mRootNode, 0, Dim::DIMENSION_BOTH, rootEntity, glm::mat4(1));


        if (p_scene->HasLights()) {
            for (unsigned int i = 0; i < p_scene->mNumLights; ++i) {
                std::cout << "Light in p_scene->mLights with name: '" << p_scene->mLights[i]->mName.C_Str() << "'" <<
                std::endl;
            }
        }

        // Remove the read file from memory
        //importer.FreeScene();
    }

    /* Regular expressions to match object dimensions */

    const std::regex REGEX_MATCH_DIMENSION_ONE = std::regex("\\S+_dim1");
    const std::regex REGEX_MATCH_DIMENSION_TWO = std::regex("\\S+_dim2");

    const Dim getDimensionOfNodeName(const char *str) {
        if (*str == 0)
            return DIMENSION_BOTH;

        if (std::regex_match(str, REGEX_MATCH_DIMENSION_ONE))
            return DIMENSION_ONE;

        if (std::regex_match(str, REGEX_MATCH_DIMENSION_TWO))
            return DIMENSION_TWO;

        return DIMENSION_BOTH;
    }

    const std::regex REGEX_MATCH_LIGHT_1 = std::regex("light_\\S+");
    const std::regex REGEX_MATCH_LIGHT_2 = std::regex("Light_\\S+");
    const std::regex REGEX_MATCH_LIGHT_3 = std::regex("lights_\\S+");
    const std::regex REGEX_MATCH_LIGHT_4 = std::regex("Lights_\\S+");

    const bool isLight(const char *str) {
        return std::regex_match(str, REGEX_MATCH_LIGHT_1) ||
               std::regex_match(str, REGEX_MATCH_LIGHT_2) ||
               std::regex_match(str, REGEX_MATCH_LIGHT_3) ||
               std::regex_match(str, REGEX_MATCH_LIGHT_4);
    }

    const std::regex REGEX_MATCH_STATIC1 = std::regex("static_\\S+");
    const std::regex REGEX_MATCH_STATIC2 = std::regex("\\S*static\\S*");

    const bool isStaticObject(const char *str) {
        return std::regex_match(str, REGEX_MATCH_STATIC1) ||
               std::regex_match(str, REGEX_MATCH_STATIC2);
    }

    const aiLight *SceneImporter::getLightWithName(const char *str) {
        if (!p_scene->HasLights())
            return nullptr;

        for (unsigned int i = 0; i < p_scene->mNumLights; ++i) {
            if (std::strcmp(p_scene->mLights[i]->mName.C_Str(), str) == 0)
                return p_scene->mLights[i];
        }

        return nullptr;
    }

    void SceneImporter::processAssimpNode(const aiNode *node,
                                          unsigned int current_depth,
                                          Dim dim_parent,
                                          ex::Entity parent,
                                          glm::mat4 parent_transform) {
        // Make sure that the supplied dim_parent is valid
        if (!(dim_parent == Dim::DIMENSION_BOTH || dim_parent == Dim::DIMENSION_ONE ||
              dim_parent == Dim::DIMENSION_TWO)) {
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

        glm::mat4 current_transform = parent_transform * aiMatrix4x4_to_glmMat4(node->mTransformation);

        /* Add the current node to its parent */
        ex::Entity current_entity = createEntity();
        current_entity.assign<TransformComponent>(current_transform);
        //current_entity.assign<GraphNodeComponent>(parent, current_entity);
        current_entity.assign<DimensionComponent>(dim_current);

        // is a light source
        if (isLight(node->mName.C_Str())) {
            const aiLight *light = getLightWithName(node->mName.C_Str());

            addLightComponentToEntity(current_entity, light);
        }
            // not a light, proceed
        else {
            // If Camera node -> node is player
            if (std::string(node->mName.C_Str()) == "Camera") {
                camera_node_ = node;
                current_entity.assign<PlayerComponent>();
                current_entity.component<DimensionComponent>()->dimension_ = sw::STARTING_DIMENSION;
            }

            // Add a MeshComponent to the entity
            if (node->mNumMeshes > 0) {
                auto transform = current_entity.component<TransformComponent>();

                unsigned int index_mesh = *(node->mMeshes);
                const aiMesh *mesh = *(p_scene->mMeshes + index_mesh);
                addMeshComponentToEntity(current_entity, mesh);
                addShadingComponentToEntity(current_entity, mesh);

                float mass = 1.0f;

                short group = btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK;
                short mask = sw::COL_STATIC;

                //if (std::string( node->mName.C_Str() ) == "Cube__2_") {
                if (isStaticObject(node->mName.C_Str())) {
                    std::cout << "STATIC\n";
                    mass = 0.0f;
                    group = sw::COL_STATIC;
                    mask = sw::COL_DYNAMIC;

                    std::cout << "mass" << std::endl;

                } else {
                    group = sw::COL_DYNAMIC;
                }

                if (current_entity.component<PlayerComponent>()) {
                    group = sw::COL_PLAYER;
                }

                addPhysicsComponentToEntity(current_entity, mass, group, mask);
            }
        }

        // Recursively process all its children nodes
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            if (*(node->mChildren + i))
                processAssimpNode(*(node->mChildren + i), current_depth + 1, dim_current, current_entity,
                                  current_transform);
        }

    }

    void SceneImporter::addMeshComponentToEntity(ex::Entity entity, const aiMesh *mesh) {
        unsigned int num = mesh->mNumVertices;

        //std::cout << "number of vertices" << num << std::endl;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        // No Textures atm...
        //std::vector<Texture> textures;

        for (unsigned int i = 0; i < num; ++i) {
            Vertex vert = {
                    aiVector_to_glmVec(*(mesh->mVertices + i)),
                    aiVector_to_glmVec(*(mesh->mNormals + i)),
                    {0.0f, 0.0f}
            };
            vertices.push_back(vert);
        }

        // Indices for each triangle
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }

        entity.assign<MeshComponent>(vertices, indices);
    }


    void SceneImporter::addShadingComponentToEntity(entityx::Entity entity, const aiMesh *mesh) {

        const aiMaterial *material = p_scene->mMaterials[mesh->mMaterialIndex];
        aiColor3D ai_ambient(0.f, 0.f, 0.f);
        aiColor3D ai_diffuse(0.f, 0.f, 0.f);
        aiColor3D ai_specular(0.f, 0.f, 0.f);
        float shininess(0.0f);

        material->Get(AI_MATKEY_COLOR_AMBIENT, ai_ambient);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_diffuse);
        material->Get(AI_MATKEY_COLOR_SPECULAR, ai_specular);
        material->Get(AI_MATKEY_SHININESS, shininess);

        sw::Color color(std::move(glm::vec3(ai_ambient.r, ai_ambient.g, ai_ambient.b)),
                        std::move(glm::vec3(ai_diffuse.r, ai_diffuse.g, ai_diffuse.b)),
                        std::move(glm::vec3(ai_specular.r, ai_specular.g, ai_specular.b)));

        entity.assign<ShadingComponent>(std::move(color), std::move(shininess));
    }

    void SceneImporter::addLightComponentToEntity(entityx::Entity entity, const aiLight *light) {

        sw::Color color(std::move(glm::vec3(light->mColorAmbient.r, light->mColorAmbient.g,
                                            light->mColorAmbient.b)),
                        std::move(glm::vec3(light->mColorDiffuse.r, light->mColorDiffuse.g,
                                            light->mColorDiffuse.b)),
                        std::move(glm::vec3(light->mColorSpecular.r, light->mColorSpecular.g,
                                            light->mColorSpecular.b)));

        LightComponent::LightType type;
        switch (light->mType) {
            case aiLightSource_POINT:
                type = LightComponent::POINT;
                break;
            case aiLightSource_DIRECTIONAL:
                type = LightComponent::DIRECTIONAL;
                break;
            case aiLightSource_SPOT:
            default:
                type = LightComponent::SPOT;
                break;
        }

        entity.assign<LightComponent>(std::move(color), type);
    }


    void SceneImporter::addPhysicsComponentToEntity(entityx::Entity entity, float mass, short group, short mask) {

        auto mesh = entity.component<MeshComponent>();
        auto transform = entity.component<TransformComponent>();

        btScalar height;

        btCollisionShape *temp = buildCollisionShape(transform->scale_, mesh->vertices, mesh->indices, height);

        entity.assign<PhysicsComponent>(entity, temp, mass, group, mask, height);


    }

    btConvexHullShape *SceneImporter::buildCollisionShape(glm::vec3 scale, std::vector<Vertex> vertices,
                                                          std::vector<GLuint> indices, btScalar &height) {


        btConvexHullShape *originalCollisionShape = new btConvexHullShape();


        glm::vec4 temp_pos;
        btVector3 current_position;
        btVector3 scaling(scale.x, scale.y, scale.z);
        btScalar maxY = -5000.f;
        btScalar minY = 5000.f;

        bool updateLocalAabb = false;

        for (Vertex vertex : vertices) {
            //temp_pos = transform * glm::vec4(vertex.Position, 1.0f);
            current_position = btVector3(vertex.Position.x, vertex.Position.y, vertex.Position.z);

            current_position *= scaling;

            /*
            std::cout << "current_position " << current_position.x() << " " << current_position.y()
            << " " << current_position.z() << std::endl;
             */

            maxY = std::max(maxY, current_position.getY());
            minY = std::min(minY, current_position.getY());


            originalCollisionShape->addPoint(current_position, updateLocalAabb);
        }

        std::cout << "Object height: " << (maxY - minY) << std::endl;
        height = maxY - minY;

        //originalCollisionShape->setLocalScaling(scaling);

        originalCollisionShape->recalcLocalAabb();
        originalCollisionShape->setMargin(0.001f);


        btShapeHull *hull = new btShapeHull(originalCollisionShape);
        //btScalar margin = originalCollisionShape->getMargin();
        hull->buildHull(originalCollisionShape->getMargin());
        //originalCollisionShape->setUserPointer(hull);

        btConvexHullShape *simplifiedConvexShape = new btConvexHullShape();

        for (int i = 0; i < hull->numVertices(); i++) {
            simplifiedConvexShape->addPoint(hull->getVertexPointer()[i], updateLocalAabb);
        }

        simplifiedConvexShape->recalcLocalAabb();

        simplifiedConvexShape->initializePolyhedralFeatures();


        //delete originalCollisionShape;
        delete hull;


        return originalCollisionShape;

    }


    glm::vec3 SceneImporter::buildBoundingVector(glm::mat4 world_transform, std::vector<Vertex> vertices) {
        // Create initial variables to hold min and max xyz values for the mesh
        glm::vec3 vert_max(FLT_MIN);
        glm::vec3 vert_min(FLT_MAX);
        glm::vec3 vertex_position;

        for (Vertex vertex : vertices) {

            vert_min = glm::min(vert_min, vertex_position);
            vert_max = glm::max(vert_max, vertex_position);

        }

        glm::vec4 vert_min_temp(vert_min, 1);
        glm::vec4 vert_max_temp(vert_max, 1);


        vert_min_temp = world_transform * vert_min_temp;

        vert_max_temp = world_transform * vert_max_temp;

        glm::vec3 diff(vert_max_temp - vert_min_temp);

        return diff / 2.0f;


    }


}