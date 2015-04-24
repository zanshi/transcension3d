//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "entityx/entityx.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "components/TransformComponent.hpp"
#include "components/RenderComponent.hpp"
#include "components/GraphNodeComponent.hpp"
#include "components/MovementComponent.hpp"

#include "events/RenderEvent.hpp"

#include "common/Shader.h"

#include "game_constants.hpp"

namespace ex = entityx;

namespace sw {

/** Placeholder rendering system that processes entities with the following components
 *      - TransformComponent
 *      - RenderComponent
 *
 *  It logs the BodyComponent's position and rotation vectors as well as the RenderComponent's debug string
 *  **/

    class RenderSystem : public ex::System<RenderSystem> {
    public:
        RenderSystem(ex::EventManager &events)
                : events_(events) {
            uniform_P = uniform_V = uniform_P = 0;

            initShader();
        };

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            // Calculate the interpolation factor alpha
            float alpha = static_cast<float>(dt / TIME_STEP);

            view_ = glm::affineInverse(view_);

            glUniformMatrix4fv(uniform_P, 1, GL_FALSE, glm::value_ptr(camera_projection_));
            glUniformMatrix4fv(uniform_V, 1, GL_FALSE, glm::value_ptr(view_));

            // Start rendering at the top of the tree
            renderEntity(root_, false, alpha, 0);
        }

        void renderEntity(ex::Entity entityToRender, bool dirty, float alpha, unsigned int current_depth) {
            auto transform = entityToRender.component<TransformComponent>();
            auto render = entityToRender.component<RenderComponent>();
            auto graphNode = entityToRender.component<GraphNodeComponent>();
            auto movement = entityToRender.component<MovementComponent>();
            auto mesh = entityToRender.component<MeshComponent>();

            // See if we need to update the current entities cached world transform
            dirty |= transform->is_dirty_;
            if (dirty) {
                // If dirty, update its cached world transform
                combine(transform, graphNode->parent_);
                transform->is_dirty_ = false;
            }

            // Render if the current entity has a RenderComponent
            if (render && mesh) {
                // The current entity can be rendered, so render it ffs
                // RENDER -> represented by emitting a RenderEvent, TODO: make it ACTUALLY render something...
                // events_.emit<RenderEvent>(entityToRender, current_depth);
                glm::mat4 model = glm::scale(transform->cached_world_, {0.1f, 0.1f, 0.1f});

                glUniformMatrix4fv(uniform_M, 1, GL_FALSE, glm::value_ptr(model));

                // Draw mesh
                glBindVertexArray(mesh->VAO);
                glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

            }

            // Render its children
            for (ex::Entity child : graphNode->children_) {
                renderEntity(child, dirty, alpha, current_depth+1);
            }
        }

        // Root entity functions

        void setRootEntity(ex::Entity root) {
            root_ = root;
            std::cout << "Root entity set." << std::endl;
        }

        void setCamera(glm::vec3 world_cameraPosition, glm::vec3 world_cameraLookAt) {
            // TODO: Validate this
            view_ = glm::lookAt(world_cameraPosition, world_cameraPosition+world_cameraLookAt, {0.0f, 0.0f, 1.0f});
        }

        void setProjectionMatrix(glm::mat4 proj) {
            camera_projection_ = proj;
        }

        void initShader() {
            shader_(); //glUseProgram

            uniform_P = glGetUniformLocation(shader_, "P");
            uniform_V = glGetUniformLocation(shader_, "V");
            uniform_M = glGetUniformLocation(shader_, "M");
        }

    private:
        // Ugly-ass hack for events
        ex::EventManager &events_;
        // Reference to the top node in the tree, where rendering ALWAYS starts
        ex::Entity root_;

        // Camera Matrix
        glm::mat4 camera_projection_;
        glm::mat4 view_;

        GLint uniform_P, uniform_V, uniform_M;
        ShaderProgram shader_ = ShaderProgram("../shaders/vertShader.vert", "../shaders/fragShader.frag");

        void combine(ex::ComponentHandle<TransformComponent> transform, ex::Entity parent_entity) {
            glm::mat4 &local = transform->local_;

            // Make sure the parent has a TransformComponent
            // TODO: Fix entity dependencies so all entities have Transform- and GraphNodeComponents
            auto transform_parent = parent_entity.component<TransformComponent>();

            if (transform_parent) {
                glm::mat4 &parent_world = transform_parent->cached_world_;
                // Multiply the local transform with the parent's world transform
                transform->cached_world_ = parent_world * local;
            } else {
                glm::mat4 parent_world(1.0f);
                // Multiply the local transform with the parent's world transform
                transform->cached_world_ = parent_world * local;
            }
        }
    };
}