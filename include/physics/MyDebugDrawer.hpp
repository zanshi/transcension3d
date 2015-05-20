//
// Created by katt on 2015-05-11.
//

#pragma once

#include "GL/glew.h"
#include "btBulletCollisionCommon.h"



namespace sw {

    struct Line {
        Line(glm::vec3 from, glm::vec3 to) : from_(from), to_(to) { };

        glm::vec3 from_;
        glm::vec3 to_;

    };


    class MyDebugDrawer : public btIDebugDraw {
    public:

        void drawLines() {

            glDisable(GL_CULL_FACE);

            std::vector <GLfloat> vertices;
            std::vector <GLuint> indices;
            GLuint index = 0;


            for(Line line : lines_) {
                vertices.push_back(line.from_.x);
                vertices.push_back(line.from_.y);
                vertices.push_back(line.from_.z);

                vertices.push_back(line.to_.x);
                vertices.push_back(line.to_.y);
                vertices.push_back(line.to_.z);

                indices.push_back(index);
                indices.push_back(index + 1);
                index += 2;

//                std::cout << "From: " << glm::to_string(line.from_) << std::endl
//                << "To: " << glm::to_string(line.to_) << std::endl;

            }

            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);
            glGenBuffers(1, &EBO_);

            btAssert(glGetError() ==GL_NO_ERROR);

            glBindVertexArray(VAO_);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat) , &vertices[0], GL_DYNAMIC_DRAW);

            btAssert(glGetError() ==GL_NO_ERROR);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

            btAssert(glGetError() ==GL_NO_ERROR);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

            btAssert(glGetError() ==GL_NO_ERROR);


            glBindVertexArray(VAO_);
            //glDrawArrays(GL_LINES, 0, lines_.size() * 2);
            //glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, (void *) &indices[0]);
            glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);

            lines_.clear();


        }


        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {

            glm::vec3 src(from.x(), from.y(), from.z());
            glm::vec3 dst(to.x(), to.y(), to.z());
            glm::vec3 col(color.x(), color.y(), color.z());

            Line l(src, dst);
            lines_.push_back(l);


        }


        virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) { }

        virtual void reportErrorWarning(const char *) { }

        virtual void draw3dText(const btVector3 &, const char *) { }

        virtual void setDebugMode(int p) {
            m = p;
        }

        int getDebugMode(void) const { return m; }

        int m;

    private:

        GLuint VAO_, VBO_, EBO_;

        std::vector<sw::Line> lines_;


    };


}


