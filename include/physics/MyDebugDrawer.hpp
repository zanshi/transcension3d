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


            /*vec3 elemens[] = {vec3(from.x(),from.y(), from.z()),  vec3(to.x(), to.y(), to.z())};

            glUseProgram(_shader.getGLProgram());

            GLuint pos = glGetAttribLocation(_shader.getGLProgram(), "a_position");
            glEnableVertexAttribArray(pos);
            glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, &elemens[0]);

            auto scene = KMDirector::getSharedDirector().getCurrenScene();
            mat4 mvmMatrix = scene->modelViewMatrix(true);
            mat4 projMatrix = KMDirector::getSharedDirector().getProjectionMatrix();
            GLuint mvmUniform  = glGetUniformLocation(_shader.getGLProgram(), "u_mvMatrix");
            GLuint projUniform = glGetUniformLocation(_shader.getGLProgram(), "u_pMatrix");

            glUniformMatrix4fv(mvmUniform, 1, 0, mvmMatrix.Pointer());
            glUniformMatrix4fv(projUniform, 1, 0, projMatrix.Pointer());

            GLuint colorUniform =  glGetUniformLocation(_shader.getGLProgram(), "u_color");
            glUniform4fv(colorUniform, 1, vec3(color.x(), color.y(), color.z()).Pointer());

            glDrawArrays(GL_LINES, 0, 2);*/




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


