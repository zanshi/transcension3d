//
// Created by Niclas Olmenius on 2015-05-11.
//

#pragma once

#include "GL/glew.h"
#include "btBulletCollisionCommon.h"


namespace sw {

    struct Line {
        Line(glm::vec3 from, glm::vec3 to);
        glm::vec3 from_;
        glm::vec3 to_;
    };


    class MyDebugDrawer : public btIDebugDraw {
    public:

        void drawLines();

        virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

        virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) override;
        virtual void reportErrorWarning(const char *) override;
        virtual void draw3dText(const btVector3 &, const char *) override;
        virtual void setDebugMode(int p) override;

        int getDebugMode(void) const override;

        int m;

    private:

        GLuint VAO_, VBO_, EBO_;
        std::vector<sw::Line> lines_;

    };


}


