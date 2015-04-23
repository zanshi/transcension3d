//
// Created by Benjamin Wiberg on 15-04-22.
//


#pragma once


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "assimp/scene.h"

// GL Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Taken from http://www.learnopengl.com/code_viewer.php?code=mesh&type=header

namespace sw {

    struct Vertex {
        // Position
        glm::vec3 Position;
        // Normal
        glm::vec3 Normal;
        // TexCoords
        glm::vec2 TexCoords;
    };

    struct Texture {
        GLuint id;
        std::string type;
        aiString path;
    };

    class MeshComponent : public ex::Component<MeshComponent> {

    public:

        MeshComponent(std::vector<Vertex> vertices = {}, std::vector<GLuint> indices = {},
                      std::vector<Texture> textures = {}) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            // Now that we have all the required data, set the vertex buffers and its attribute pointers.

            this->setupMesh();

        };

        /*  Mesh Data  */
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        /*  Render data  */
        GLuint VAO, VBO, EBO;

    private:

        void setupMesh()
        {
            // Create buffers/arrays
            glGenVertexArrays(1, &this->VAO);
            glGenBuffers(1, &this->VBO);
            glGenBuffers(1, &this->EBO);

            glBindVertexArray(this->VAO);
            // Load data into vertex buffers
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            // A great thing about structs is that their memory layout is sequential for all its items.
            // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
            // again translates to 3/2 floats which translates to a byte array.
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

            // Set the vertex attribute pointers
            // Vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
            // Vertex Normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
            // Vertex Texture Coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }


    };


}