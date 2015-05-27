//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "entityx/entityx.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "components/all_components.hpp"

#include "events/RenderEvent.hpp"
#include "events/StartDimensionChangeEvent.hpp"
#include "events/DimensionChangeInProgressEvent.hpp"

#include "common/Shader.h"
#include "rendering/PointShadowDepthBuffer.hpp"

#include "game_constants.hpp"
#include "PhysicsSystem.hpp"

#include <iomanip>
// for pow(float, float)
#include <math.h>

#include <unordered_map> // For saving shader uniforms

namespace ex = entityx;

namespace sw {

/** Placeholder rendering system that processes entities with the following components
 *      - TransformComponent
 *      - RenderComponent
 *
 *  It logs the BodyComponent's position and rotation vectors as well as the RenderComponent's debug string
 *  **/

    class RenderSystem : public ex::System<RenderSystem>, public ex::Receiver<RenderSystem> {
    public:
        RenderSystem(ex::EventManager &events) {
            P_loc = V_loc = P_loc = 0;
            camera_projection_ = glm::perspective(glm::radians(60.f * 0.75f), 800.0f / 600.0f, 0.01f, 10.0f);
            num_dir_lights_ = num_point_lights_ = 0;

            current_dim_ = sw::STARTING_DIMENSION;
            dim_change_in_progress_ = false;

            events.subscribe<StartDimensionChangeEvent>(*this);
            events.subscribe<DimensionChangeInProgressEvent>(*this);
        };

        ~RenderSystem() {
            delete shader_;
        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            if (!has_received_dimension_in_progress_event_) {
                dim_change_in_progress_ = false;
            }

            // Calculate the interpolation factor alpha
            float alpha = static_cast<float>(dt / TIME_STEP);

            // Render all entities from each point light's view
            updateShadowMaps(es);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Use the "standard" shader for rendering the stuff seen on screen
            (*shader_)();
            glUniformMatrix4fv(P_loc, 1, GL_FALSE, glm::value_ptr(camera_projection_));

            // Update all uniforms for player location, light locations and intensities etc...
            updatePlayerData(es, dt);
            updateSceneLightsData(es, dt);

            // Render all entities to be displayed on screen
            renderAllEntities(es);

            std::cout << "glGetError: " << glGetError() << "\n";

            // Revert to old state
            has_received_dimension_in_progress_event_ = false;
        }

        // Root entity functions
        void setRootEntity(ex::Entity root) {
            root_ = root;
            std::cout << "Root entity set." << std::endl;
        }

        void init(ex::EntityManager &es) {
            initShader();
            initShadowMappingShader();
            initPointShadowBuffers(es);
        }

        void receive(const StartDimensionChangeEvent &startChange) {
            if (!dim_change_in_progress_) {
                dim_from_ = current_dim_;
            }
        }

        void receive(const DimensionChangeInProgressEvent &dimChange) {
            has_received_dimension_in_progress_event_ = true;
            dim_change_in_progress_ = true;

            if (current_dim_ == dim_from_ && dimChange.completion_factor_ >= 0.5f) {
                current_dim_ = (current_dim_ == Dim::DIMENSION_ONE) ? Dim::DIMENSION_TWO : Dim::DIMENSION_ONE;
            }

            float scale = generateEaseScale(dimChange.completion_factor_);

            dim_change_scale_mat_ = glm::scale(glm::mat4(1), glm::vec3(scale, scale, scale));
        }

    private:
        void initShadowMappingShader() {
            shadow_shader_ = new ShaderProgram("../shaders/SM_Point.vert",
                                               "../shaders/SM_Point.frag",
                                               "../shaders/SM_Point.geom");
            (*shadow_shader_)();

            shadow_model_loc = glGetUniformLocation(*shadow_shader_, "model");
            shadow_lightPos_loc = glGetUniformLocation(*shadow_shader_, "lightPos");
            shadow_far_plane_loc = glGetUniformLocation(*shadow_shader_, "far_plane");

            std::cout << shadow_model_loc << ", " << shadow_lightPos_loc << ", " << shadow_far_plane_loc << "\n";

            for (int i = 0; i < 6; ++i) {
                std::string uniform = "shadowMatrices[" + std::to_string(i) + "]";
                shadowMatrices_loc[i] = glGetUniformLocation(*shadow_shader_, uniform.c_str());
                std::cout << uniform << " = " << shadowMatrices_loc[i] << "\n";
            }

            std::cout << "Errors after glGetUniformLocation: " << glGetError() << ", " << glGetError() << "\n";
        }

        void initPointShadowBuffers(ex::EntityManager &es) {
            // Get all the lights
            auto dimension = ex::ComponentHandle<DimensionComponent>();
            auto light = ex::ComponentHandle<LightComponent>();
            auto transform = ex::ComponentHandle<TransformComponent>();

            for (ex::Entity e : es.entities_with_components(dimension, light, transform)) {
                if (light->type_ != LightComponent::LightType::POINT)
                    continue;

                // Add a PSDB to each point light as a component
                (e.assign<PointShadowDepthBuffer>(transform->position_))->init();
            }
        }

        void initShader() {
            shader_ = new ShaderProgram("../shaders/vertShader.vert",
                                          "../shaders/fragShader.frag");
            (*shader_)(); //glUseProgram

            //Model-View-Projection
            P_loc = glGetUniformLocation(*shader_, "P");
            V_loc = glGetUniformLocation(*shader_, "V");
            V_inv_loc = glGetUniformLocation(*shader_, "V_inv");
            M_loc = glGetUniformLocation(*shader_, "M");

            /* LIGHTING SETUP */
            // Uniform locations for point lights
            for (unsigned int light = 0; light < MAX_POINT_LIGHTS; light++) {
                for (unsigned int attr = 0; attr < POINT_LIGHT_ATTRS; ++attr) {
                    std::string last_part = "";
                    switch (attr) {
                        case 0:
                            last_part = "position";
                            break;
                        case 1:
                            last_part = "ambient";
                            break;
                        case 2:
                            last_part = "diffuse";
                            break;
                        case 3:
                            last_part = "specular";
                            break;
                        default:
                            last_part = "";
                            break;
                    }
                    std::string uniform = "pointLights[" + std::to_string(light) + "]." + last_part;
                    pointLightsLoc[light][attr] = glGetUniformLocation(*shader_, uniform.c_str());

                    uniform = "depthCubeMaps[" + std::to_string(light) + "]";
                    depthCubemaps_loc[light] = glGetUniformLocation(*shader_, uniform.c_str());
                }
            }
            // Uniform locations for directional lights
            for (unsigned int light = 0; light < MAX_DIR_LIGHTS; light++) {
                for (unsigned int attr = 0; attr < DIR_LIGHT_ATTRS; ++attr) {
                    std::string last_part = "";
                    switch (attr) {
                        case 0:
                            last_part = "position";
                            break;
                        case 1:
                            last_part = "direction";
                            break;
                        case 2:
                            last_part = "ambient";
                            break;
                        case 3:
                            last_part = "diffuse";
                            break;
                        case 4:
                            last_part = "specular";
                            break;
                        default:
                            last_part = "";
                            break;
                    }
                    std::string uniform = "dirLights[" + std::to_string(light) + "]." + last_part;
                    dirLightsLoc[light][attr] = glGetUniformLocation(*shader_, uniform.c_str());
                }
            }

            num_point_lights_loc = glGetUniformLocation(*shader_, "numPointLights");
            num_dir_lights_loc = glGetUniformLocation(*shader_, "numDirLights");

            //Material
            matlAmbientLoc = glGetUniformLocation(*shader_, "material.ambient");
            matlDiffuseLoc = glGetUniformLocation(*shader_, "material.diffuse");
            matlSpecularLoc = glGetUniformLocation(*shader_, "material.specular");
            matlShineLoc = glGetUniformLocation(*shader_, "material.shininess");

            viewPosLoc = glGetUniformLocation(*shader_, "viewPos");

            // Shadow mapping
            standard_far_plane_loc = glGetUniformLocation(*shader_, "far_plane");
        }


        void updateSceneLightsData(ex::EntityManager &es, ex::TimeDelta dt) {
            /* LIGHTS */
            // Get all the lights
            auto dimension = ex::ComponentHandle<DimensionComponent>();
            auto light = ex::ComponentHandle<LightComponent>();
            auto transform = ex::ComponentHandle<TransformComponent>();
            auto psdb = ex::ComponentHandle<PointShadowDepthBuffer>();

            num_point_lights_ = num_dir_lights_ = 0;
            for (ex::Entity current_light : es.entities_with_components(transform, dimension, light, psdb)) {
                // Early bailout: is the current light in the current dimension?
                if (!(dimension->dimension_ == current_dim_ || dimension->dimension_ == Dim::DIMENSION_BOTH))
                    continue;

                // All these values are in the Eye's coordinate system
                glm::mat4 l_transform = transform->cached_world_;
                glm::vec3 l_pos;
                glm::quat l_quat;
                glm::decompose(l_transform, temp3, l_quat, l_pos, temp3, temp4);
                //std::cout << "Light pos: x=" << lightEyePos[0] << ", y=" << lightEyePos[1] << ", z=" << lightEyePos[2] << std::endl;
                glm::vec3 l_euler = glm::eulerAngles(l_quat);

                switch (light->type_) {
                    case LightComponent::LightType::POINT: {
                        // Position
                        glUniform3f(pointLightsLoc[num_point_lights_][0], l_pos.x, l_pos.y, l_pos.z);

                        // Ambient, Diffuse, Specular
                        Color &c = light->color_;
                        glUniform3f(pointLightsLoc[num_point_lights_][1], c.ambient_.x, c.ambient_.y, c.ambient_.z);
                        glUniform3f(pointLightsLoc[num_point_lights_][2], c.diffuse_.x, c.diffuse_.y, c.diffuse_.z);
                        glUniform3f(pointLightsLoc[num_point_lights_][3], c.specular_.x, c.specular_.y, c.specular_.z);

                        /* Shadow Mapping uniforms */
                        int iTextureUnit = num_point_lights_;
                        glActiveTexture(GL_TEXTURE0 + iTextureUnit);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, psdb->Cubemap());

                        /*
                        glBindSampler(iTextureUnit, uiSampler);

                        // samplerCube i.e. the Cubemap
                        glUniform1i(depthCubemaps_loc[0], 4);
                        glActiveTexture(GL_TEXTURE0 + 4);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, psdb->Cubemap());

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_CUBE_MAP, psdb->Cubemap());
                         */
                    };
                        num_point_lights_++;
                        break;
                    case LightComponent::LightType::DIRECTIONAL: {
                        // Position, Direction
                        glUniform3f(dirLightsLoc[num_dir_lights_][0], l_pos.x, l_pos.y, l_pos.z);
                        glUniform3f(dirLightsLoc[num_dir_lights_][1], 0.0f, 0.0f, 0.0f); //TODO (bwiberg): real vals

                        // Ambient, Diffuse, Specular
                        Color &c = light->color_;
                        glUniform3f(dirLightsLoc[num_dir_lights_][2], c.ambient_.x, c.ambient_.y, c.ambient_.z);
                        glUniform3f(dirLightsLoc[num_dir_lights_][3], c.diffuse_.x, c.diffuse_.y, c.diffuse_.z);
                        glUniform3f(dirLightsLoc[num_dir_lights_][4], c.specular_.x, c.specular_.y, c.specular_.z);
                    };
                        num_dir_lights_++;
                        break;
                    default:
                        break;
                }
            }

            // Send the shadow mapping far_plane clipping distance
            glUniform1f(standard_far_plane_loc, psdb->FAR_PLANE_);

            // Send how many lights of each type there are this frame
            glUniform1i(num_point_lights_loc, num_point_lights_);
            glUniform1i(num_dir_lights_loc, num_dir_lights_);
        }

        void updatePlayerData(ex::EntityManager &es, ex::TimeDelta dt) {
            bool foundPlayer = false;

            // Get the player's position, i.e. the camera's position
            auto player = ex::ComponentHandle<PlayerComponent>();
            auto transform = ex::ComponentHandle<TransformComponent>();
            auto physics = ex::ComponentHandle<PhysicsComponent>();

            for (ex::Entity ex : es.entities_with_components(player, transform, physics)) {
                foundPlayer = true;

                btTransform worldTransform;
                physics->motionState_->getWorldTransform(worldTransform);

                glm::mat4 player_transform_world;

                worldTransform.getOpenGLMatrix(glm::value_ptr(player_transform_world));

                view_ = player_transform_world * glm::yawPitchRoll(-player->yaw_, 0.0f, 0.0f) *
                        glm::yawPitchRoll(0.0f, -player->pitch_, 0.0f);
            }

            assert(foundPlayer && "Error: No player in the scene, aborting.");

            glUniformMatrix4fv(V_loc, 1, GL_FALSE, glm::value_ptr(view_));
            glm::mat4 view_inverse = glm::affineInverse(view_);
            glUniformMatrix4fv(V_inv_loc, 1, GL_FALSE, glm::value_ptr(view_inverse));

            // Pass the position of the eye in world coordinates
            glm::vec3 viewPos;
            glm::decompose(view_, temp3, tempquat, viewPos, temp3, temp4);
            glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
        }

        void renderAllEntities(ex::EntityManager &es, bool shadowMapRendering = false) {
            auto dimension = ex::ComponentHandle<DimensionComponent>();
            auto physics = ex::ComponentHandle<PhysicsComponent>();
            auto mesh = ex::ComponentHandle<MeshComponent>();
            auto shading = ex::ComponentHandle<ShadingComponent>();

            // Render all entities in the order determined by EntityX
            for (ex::Entity e : es.entities_with_components(dimension, physics, mesh, shading)) {
                // Early bailout: is the current entity in the current dimension?
                if (dimension &&
                    !(dimension->dimension_ == current_dim_ || dimension->dimension_ == Dim::DIMENSION_BOTH))
                    continue;
                // Don't render the player
                if (e.component<PlayerComponent>().valid())
                    continue;

                renderEntity(dimension, physics, mesh, shading, shadowMapRendering);
            }
        }

        void renderMesh(ex::ComponentHandle<MeshComponent> mesh) {
            // Draw mesh
            glBindVertexArray(mesh->VAO);
            // TODO: Fix "Conditional jump or move depends on uninitialised value(s)"
            glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        void renderEntity(ex::ComponentHandle<DimensionComponent> dimension,
                          ex::ComponentHandle<PhysicsComponent> physics,
                          ex::ComponentHandle<MeshComponent> mesh,
                          ex::ComponentHandle<ShadingComponent> shading,
                          bool shadowMapRendering = false) {
            GLint modelMatrix_loc;

            if (!shadowMapRendering) {
                Color &c = shading->color_;

                // Set object material properties
                glUniform3f(matlAmbientLoc, c.ambient_.r, c.ambient_.g,
                            c.ambient_.b);
                glUniform3f(matlDiffuseLoc, c.diffuse_.r, c.diffuse_.g,
                            c.diffuse_.b);
                glUniform3f(matlSpecularLoc, c.specular_.r, c.specular_.g,
                            c.specular_.b);
                glUniform1f(matlShineLoc, shading->shininess_);

                modelMatrix_loc = M_loc;
            }

            else {
                modelMatrix_loc = shadow_model_loc;
            }


            // Houston, we can render.
            btTransform worldTransform;
            physics->motionState_->getWorldTransform(worldTransform);

            glm::mat4 model;
            worldTransform.getOpenGLMatrix(glm::value_ptr(model));

            // Update it with scaling according to dimensional change! YEAH
            if (dim_change_in_progress_) {
                if (current_dim_ == dimension->dimension_) {
                    model = model * dim_change_scale_mat_;
                }
            }

            glUniformMatrix4fv(modelMatrix_loc, 1, GL_FALSE, glm::value_ptr(model));

            renderMesh(mesh);
        }

        void updateShadowMaps(ex::EntityManager &es) {
            auto light = ex::ComponentHandle<LightComponent>();
            auto transform = ex::ComponentHandle<TransformComponent>();
            auto dimension = ex::ComponentHandle<DimensionComponent>();
            auto psdb = ex::ComponentHandle<PointShadowDepthBuffer>();

            for (ex::Entity e : es.entities_with_components(light, transform, dimension, psdb)) {
                // Early bailout: is the current entity in the current dimension?
                if (dimension &&
                    !(dimension->dimension_ == current_dim_ || dimension->dimension_ == Dim::DIMENSION_BOTH))
                    continue;

                // Do some shit with shadow mapping
                (*shadow_shader_)(); // glUseProgram()
                psdb->bindForRendering(); // glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

                // Feed uniforms to the shader
                for (unsigned int i = 0; i < 6; ++i)
                    glUniformMatrix4fv(shadowMatrices_loc[i], 1, GL_FALSE, psdb->getShadowMatrixValuePtr(i));

                glm::vec3 &pos = transform->world_position_;
                glUniform3f(shadow_lightPos_loc, pos.x, pos.y, pos.z);
                glUniform1f(shadow_far_plane_loc, psdb->FAR_PLANE_);

                renderAllEntities(es, true);
                psdb->unbind(); // glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }

        float generateEaseScale(float time) {
            return 0.5f + 0.5f * cosf(2 * M_PI * time);
            //return 0.5f + 0.5f * cosf(M_PI * (1.0f - scale));
        }

        /* GAME-RELATED VARIABLES */
        Dim current_dim_;

        Dim dim_from_;
        bool dim_change_in_progress_;
        bool has_received_dimension_in_progress_event_ = false;
        glm::mat4 dim_change_scale_mat_;

        /* RENDER-RELATED VARIABLES */
        ShaderProgram *shader_;

        // Reference to the top node in the tree, where rendering ALWAYS starts
        ex::Entity root_;
        // Projection matrix and view / camera / player matrix transform
        glm::mat4 camera_projection_;

        glm::mat4 view_;
        /* Lighting */
        static const int MAX_POINT_LIGHTS = 20, MAX_DIR_LIGHTS = 20;
        static const int POINT_LIGHT_ATTRS = 4, DIR_LIGHT_ATTRS = 5;

        // Keeps track of how many lights of each type there are currently
        int num_point_lights_, num_dir_lights_, num_spot_lights;

        // Uniform variables locations
        GLint pointLightsLoc[MAX_POINT_LIGHTS][POINT_LIGHT_ATTRS];
        GLint dirLightsLoc[MAX_DIR_LIGHTS][DIR_LIGHT_ATTRS];
        GLint num_point_lights_loc, num_dir_lights_loc;

        GLint viewPosLoc;
        //Material
        GLint matlAmbientLoc, matlDiffuseLoc, matlSpecularLoc, matlShineLoc;

        GLint depthCubemaps_loc[MAX_POINT_LIGHTS];
        GLint standard_far_plane_loc;

        // Matrices
        GLint P_loc, V_loc, V_inv_loc, M_loc;

        // Temp variables, used (and reused) for GLM decomposition. Reused to remove need to redeclare them each time
        glm::vec3 temp3;
        glm::vec4 temp4;
        glm::quat tempquat;

        // Shadow mapping: DepthBuffer objects
        ShaderProgram *shadow_shader_;
        GLint shadow_model_loc, shadow_lightPos_loc, shadow_far_plane_loc;
        GLint shadowMatrices_loc[6];

        void combine(ex::ComponentHandle<TransformComponent> transform, ex::Entity parent_entity) {
            glm::mat4 local = transform->local_;

            // Make sure the parent has a TransformComponent
            // TODO: Fix entity dependencies so all entities have Transform- and GraphNodeComponents
            auto transform_parent = parent_entity.component<TransformComponent>();

            if (transform_parent) {
                glm::mat4 parent_world = transform_parent->cached_world_;
                transform->cached_world_ = parent_world * local;
            } else {
                glm::mat4 parent_world(1.0f);
                // Multiply the local transform with the parent's world transform
                transform->cached_world_ = parent_world * local;
            }
            // Multiply the local transform with the parent's world transform

        }

        /* DEBUGGING FUNCTIONS */

        void print_glmMatrix(glm::mat4 pMat4) {

            double dArray[16] = {0.0};
            const float *pSource = (const float *) glm::value_ptr(glm::transpose(pMat4));
            for (int i = 0; i < 16; ++i) {
                dArray[i] = pSource[i];
                std::cout << std::fixed << std::setprecision(2);
                std::cout << dArray[i] << " ";
                if (i == 3 || i == 7 || i == 11)
                    std::cout << std::endl;
            }
            std::cout << std::endl << "--------------------" << std::endl;

        }

        void print_glmVec3(glm::vec3 vec) {
            for (int i = 0; i < 3; ++i) {
                std::cout << std::fixed << std::setprecision(2);
                std::cout << vec[i] << " ";
            }
            std::cout << std::endl << "--------------------" << std::endl;
        }

    };
}