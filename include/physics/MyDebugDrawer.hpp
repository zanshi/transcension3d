//
// Created by katt on 2015-05-11.
//

#pragma once

#include "GL/glew.h"
#include "btBulletCollisionCommon.h"


namespace sw {


    class MyDebugDrawer : public btIDebugDraw {


    public:

        virtual ~MyDebugDrawer(){

        }

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {


            GLuint VertexArrayID;
            glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);


            const GLfloat g_vertex_buffer_data[] = {
                    from.getX(), from.getY(), from.getZ(),
                    to.getX(), to.getY(), to.getZ(),
                    from.getX(), from.getY(), from.getZ()
            };

            // This will identify our vertex buffer
            GLuint vertexbuffer;

            // Generate 1 buffer, put the resulting identifier in vertexbuffer
            glGenBuffers(1, &vertexbuffer);

            // The following commands will talk about our 'vertexbuffer' buffer
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

            // Give our vertices to OpenGL.
            glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(
                    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
            );

            // Draw the triangle !
            glDrawArrays(GL_LINE_STRIP, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

            glDisableVertexAttribArray(0);

            glBindVertexArray(0);

        }


        virtual void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance,
                                      int lifeTime, const btVector3 &color) {

        }

        virtual void reportErrorWarning(const char *warningString) {

        }

        virtual void draw3dText(const btVector3 &location, const char *textString) {

        }

        virtual void setDebugMode(int debugMode) {
            debugmode_ = (DebugDrawModes) debugMode;
        }

        virtual int getDebugMode() const {
            return debugmode_;
        }


    private:
        int debugmode_ = btIDebugDraw::DBG_DrawWireframe;

    };

}


