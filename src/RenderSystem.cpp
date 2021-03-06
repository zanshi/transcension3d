//
// Created by Benjamin Wiberg on 15-05-30.
//

#include "systems/RenderSystem.hpp"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include "common/Shader.h"

#include "game_constants.hpp"

namespace ex = entityx;

sw::RenderSystem::RenderSystem(ex::EventManager & events) {
    P_loc = V_loc = P_loc = 0;
    camera_projection_ = glm::perspective(glm::radians(sw::FOVY_DEGREES), 800.0f / 600.0f, 0.01f, 10.0f);
    num_dir_lights_ = num_point_lights_ = 0;

    current_dim_ = sw::STARTING_DIMENSION;
    dim_change_in_progress_ = false;

    events.subscribe<StartDimensionChangeEvent>(*this);
    events.subscribe<DimensionChangeInProgressEvent>(*this);
};

sw::RenderSystem::~RenderSystem() {
    delete shader_;
};

void sw::RenderSystem::updateSceneLightsData(ex::EntityManager &es, ex::TimeDelta dt) {
    /* LIGHTS */
    // Get all the lights
    auto dimension = ex::ComponentHandle<DimensionComponent>();
    auto light = ex::ComponentHandle<LightComponent>();
    auto transform = ex::ComponentHandle<TransformComponent>();
    // auto graphNode = ex::ComponentHandle<GraphNodeComponent>();

    num_point_lights_ = num_dir_lights_ = 0;
    for (ex::Entity current_light : es.entities_with_components(transform, dimension, light)) {
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

    // Send how many lights of each type there are this frame
    glUniform1i(num_point_lights_loc, num_point_lights_);
    glUniform1i(num_dir_lights_loc, num_dir_lights_);
}

void sw::RenderSystem::updatePlayerData(ex::EntityManager &es, ex::TimeDelta dt) {
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
};

void sw::RenderSystem::renderEntity(ex::ComponentHandle < DimensionComponent > dimension,
                                    ex::ComponentHandle < PhysicsComponent > physics,
                                    ex::ComponentHandle < MeshComponent > mesh,
                                    ex::ComponentHandle < ShadingComponent > shading) {
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
    glUniformMatrix4fv(M_loc, 1, GL_FALSE, glm::value_ptr(model));

    Color &c = shading->color_;

    // Set object material properties
    glUniform3f(matlAmbientLoc, c.ambient_.r, c.ambient_.g,
                c.ambient_.b);
    glUniform3f(matlDiffuseLoc, c.diffuse_.r, c.diffuse_.g,
                c.diffuse_.b);
    glUniform3f(matlSpecularLoc, c.specular_.r, c.specular_.g,
                c.specular_.b);
    glUniform1f(matlShineLoc, shading->shininess_);

    // Draw mesh
    glBindVertexArray(mesh->VAO);
    // TODO: Fix "Conditional jump or move depends on uninitialised value(s)"
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void sw::RenderSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
    if (!has_received_dimension_in_progress_event_) {
        dim_change_in_progress_ = false;
    }

    // Calculate the interpolation factor alpha
    float alpha = static_cast<float>(dt / TIME_STEP);

    glUniformMatrix4fv(P_loc, 1, GL_FALSE, glm::value_ptr(camera_projection_));

    updatePlayerData(es, dt);
    updateSceneLightsData(es, dt);

    auto dimension = ex::ComponentHandle<DimensionComponent>();
    auto physics = ex::ComponentHandle<PhysicsComponent>();
    auto mesh = ex::ComponentHandle<MeshComponent>();
    auto shading = ex::ComponentHandle<ShadingComponent>();

    // Render all entities in the order determined by EntityX
    for (ex::Entity e : es.entities_with_components(dimension, physics, mesh, shading)) {
        // Early bailout: is the current entity in the current dimension?
        if (dimension && !(dimension->dimension_ == current_dim_ || dimension->dimension_ == Dim::DIMENSION_BOTH))
            continue;
        // Don't render the player
        if (e.component<PlayerComponent>().valid())
            continue;

        renderEntity(dimension, physics, mesh, shading);
    }

    // Revert to old state
    has_received_dimension_in_progress_event_ = false;

    events.emit<ProjectionViewEvent>(camera_projection_, view_);

};

void sw::RenderSystem::initShader() {

    shader_ = new ShaderProgram("../shaders/vertShader.vert", "../shaders/fragShader.frag");
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
};

void sw::RenderSystem::receive(const StartDimensionChangeEvent &startChange) {
    if (!dim_change_in_progress_) {
        dim_from_ = current_dim_;
    }
};

float sw::RenderSystem::generateEaseScale(float time) {
    return 0.5f + 0.5f * cosf(2 * M_PI * time);
    //return 0.5f + 0.5f * cosf(M_PI * (1.0f - scale));
};

void sw::RenderSystem::receive(const DimensionChangeInProgressEvent &dimChange) {
    has_received_dimension_in_progress_event_ = true;
    dim_change_in_progress_ = true;

    if (current_dim_ == dim_from_ && dimChange.completion_factor_ >= 0.5f) {
        current_dim_ = (current_dim_ == Dim::DIMENSION_ONE) ? Dim::DIMENSION_TWO : Dim::DIMENSION_ONE;
    }

    // Generate the scale matrix to use
    /*
    float scale = 0.0f;
    if (dimChange.completion_factor_ < 0.5f) {
        scale = (0.5f - dimChange.completion_factor_) / 0.5f;
    }
    else if (dimChange.completion_factor_ >= 0.5f) {
        scale = (dimChange.completion_factor_ - 0.5f) / 0.5f;
    }
    else {
        scale = 1.0f;
    }
     */

    float scale = generateEaseScale(dimChange.completion_factor_);

    dim_change_scale_mat_ = glm::scale(glm::mat4(1), glm::vec3(scale, scale, scale));
};

void sw::RenderSystem::combine(ex::ComponentHandle < TransformComponent > transform, ex::Entity parent_entity) {
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

};