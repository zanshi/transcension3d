//
// Created by Benjamin Wiberg on 15-05-25.
//


#pragma once

#include "GL/glew.h"

#include "SDL.h"

namespace sw {
    class PointShadowDepthBuffer {
    public:
        PointShadowDepthBuffer(const glm::vec3 light_world_pos = {0.0f, 0.0f, 0.0f},
                               const GLuint width = 256,
                               const GLuint height = 256)
                : light_world_pos_(light_world_pos),
                  WIDTH_(width),
                  HEIGHT_(height),
                  has_updated_light_pos_(true) { }

        void init() {
            glGenFramebuffers(1, &depthMapFBO);

            // Depth buffer is cube map, 6 faces
            glGenTextures(1, &depthCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
            for (GLuint i = 0; i < 6; ++i)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                             WIDTH_, HEIGHT_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            // Attach cubemap as the depthbuffer's depth texture map
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Generate projection matrix
            GLfloat aspect = (GLfloat) WIDTH_ / (GLfloat) HEIGHT_;
            GLfloat near = 1.0f;
            GLfloat far = 25.0f;
            projection_ = glm::perspective(90.0f, aspect, near, far);
        }

        void configureMatrices() {
            has_updated_light_pos_ = false;
            shadowTransforms.clear();

            shadowTransforms.push_back(projection_ *
                                       glm::lookAt(light_world_pos_, light_world_pos_ + glm::vec3(1.0, 0.0, 0.0),
                                                   glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(projection_ *
                                       glm::lookAt(light_world_pos_, light_world_pos_ + glm::vec3(-1.0, 0.0, 0.0),
                                                   glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(projection_ *
                                       glm::lookAt(light_world_pos_, light_world_pos_ + glm::vec3(0.0, 1.0, 0.0),
                                                   glm::vec3(0.0, 0.0, 1.0)));
            shadowTransforms.push_back(projection_ *
                                       glm::lookAt(light_world_pos_, light_world_pos_ + glm::vec3(0.0, -1.0, 0.0),
                                                   glm::vec3(0.0, 0.0, -1.0)));
            shadowTransforms.push_back(projection_ *
                                       glm::lookAt(light_world_pos_, light_world_pos_ + glm::vec3(0.0, 0.0, 1.0),
                                                   glm::vec3(0.0, -1.0, 0.0)));
            shadowTransforms.push_back(projection_ *
                                       glm::lookAt(light_world_pos_, light_world_pos_ + glm::vec3(0.0, 0.0, -1.0),
                                                   glm::vec3(0.0, -1.0, 0.0)));
        }

        void bindForRendering() {
            if (has_updated_light_pos_) {
                configureMatrices();
            }

            // Bind for rendering to the depth map
            glViewport(0, 0, WIDTH_, HEIGHT_);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        void unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, sw::WINDOW_WIDTH, sw::WINDOW_HEIGHT);
        }

        void updateLightPosition(glm::vec3 new_world_pos) {
            light_world_pos_ = new_world_pos;
            has_updated_light_pos_ = true;
        }

        const GLuint WIDTH_, HEIGHT_;

    private:
        GLuint depthMapFBO;
        GLuint depthCubemap;

        glm::mat4 projection_;
        glm::vec3 light_world_pos_;
        bool has_updated_light_pos_;

        std::vector <glm::mat4> shadowTransforms;
    };
}