//
// Created by Benjamin Wiberg on 15-04-22.
//

#include "scene/SceneImporter.hpp"
#include <regex>

// GL Includes
#include <GL/glew.h>

// GLM includes
#include <glm/glm.hpp>

// Project-related imports
#include "components/all_components.hpp"

namespace sw {
    std::string SceneImporter::relative_path_to_scene_folder_ = "../res/";

    SceneImporter::SceneImporter(std::string filename) {
        filename = relative_path_to_scene_folder_ + filename;
        p_scene = importer.ReadFile(filename.c_str(), 0);
        if (p_scene == nullptr) {
            std::cout << "The file could not be loaded. Exiting." << std::endl;
            assert(p_scene);
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
        processAssimpNode(p_scene->mRootNode, 0, Dim::DIMENSION_BOTH, rootEntity);

        // Remove the read file from memory
        importer.FreeScene();

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

    void SceneImporter::processAssimpNode(const aiNode *node,
                                          unsigned int current_depth,
                                          Dim dim_parent,
                                          ex::Entity parent) {

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

        /* Add the current node to its parent */
        ex::Entity current_entity = createEntity();
        current_entity.assign<TransformComponent>(aiMatrix4x4_to_glmMat4(node->mTransformation));
        current_entity.assign<GraphNodeComponent>(parent, current_entity);
        current_entity.assign<DimensionComponent>(dim_current);
        current_entity.assign<RenderComponent>(std::string(node->mName.C_Str()));

        // Add a MeshComponent to the entity
        if (node->mMeshes) {
            unsigned int index_mesh = *(node->mMeshes);
            const aiMesh *mesh = *(p_scene->mMeshes+index_mesh);
            addMeshComponentToEntity(current_entity, mesh);
        }

        // Recursively process all its children nodes
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            if (*(node->mChildren + i))
                processAssimpNode(*(node->mChildren + i), current_depth + 1, dim_current, current_entity);
        }
    }

    /* Mesh methods and private helper functions */
    glm::vec3 aiVector_to_glmVec(const aiVector3D in_vec) {
        return glm::vec3(in_vec[0], in_vec[1], in_vec[2]);
    }

    glm::vec2 aiVector_to_glmVec(const aiVector2D in_vec) {
        return glm::vec2(in_vec[0], in_vec[1]);
    }

    void SceneImporter::addMeshComponentToEntity(ex::Entity entity, const aiMesh *mesh) {
        unsigned int num = mesh->mNumVertices;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        // No Textures atm...
        //std::vector<Texture> textures;

        for (unsigned int i = 0; i < num; ++i) {
            Vertex vert = {
                    aiVector_to_glmVec( *(mesh->mVertices+i) ),
                    aiVector_to_glmVec( *(mesh->mNormals+i) ),
                    {0.0f, 0.0f}
            };
            GLuint index = VERTEX_NUM_ATTRIBUTES * i;

            vertices.push_back(vert);
            indices.push_back(index);
        }

        entity.assign<MeshComponent>(vertices, indices);
    }
}