//
// Created by cristoffer on 2015-05-26.
//



#include "entityx/entityx.h"
#include "systems/PlayerControlSystem.hpp"
#include "components/PlayerComponent.hpp"
#include "components/DimensionComponent.hpp"
#include "components/PhysicsComponent.hpp"
#include "components/DimensionComponent.hpp"

#include "game_constants.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <cmath>


sw::PlayerControlSystem::PlayerControlSystem() {
    current_dim_ = sw::STARTING_DIMENSION;
    dim_from_ = current_dim_;
    dim_change_counter_ = 0;
    last_dim_change_ = -1;
}

void sw::PlayerControlSystem::configure(ex::EventManager & events) {
    events.subscribe<JumpEvent>(*this);
    events.subscribe<MovementEvent>(*this);
    events.subscribe<ViewChangedEvent>(*this);
    events.subscribe<DimensionChangeInProgressEvent>(*this);
    events.subscribe<StartDimensionChangeEvent>(*this);
}

void sw::PlayerControlSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
    auto player = ex::ComponentHandle<PlayerComponent>();
    auto transform = ex::ComponentHandle<TransformComponent>();
    auto physics = ex::ComponentHandle<PhysicsComponent>();
    auto dimension = ex::ComponentHandle<DimensionComponent>();

    for (ex::Entity entity : es.entities_with_components(player, transform, physics, dimension)) {
        //Update the player

        /* Rotation */
        if (will_change_view_) {
            // X-rotation a.k.a. yaw
            player->yaw_ += delta_yaw_;

            // Y-rotation a.k.a. pitch, "looking up or down"
            player->pitch_ = pitch_;

            will_change_view_ = false;
        }
        float oldY = physics->body_->getLinearVelocity().y();

        /* Movement */
        if (will_move_) {
            btTransform worldTransform;
            physics->motionState_->getWorldTransform(worldTransform);

            glm::mat4 player_transform_world;

            worldTransform.getOpenGLMatrix(glm::value_ptr(player_transform_world));

            glm::mat4 view_mat = player_transform_world * glm::yawPitchRoll(-player->yaw_, 0.0f, 0.0f);

            glm::vec3 player_move =
                    move_forward_ * glm::vec3(0.0f, 0.0f, -1.0f) + move_right_ * glm::vec3(1.0f, 0.0f, 0.0f);

            float norm_fac = sqrtf(exp2f(player_move.x) + exp2f(player_move.y) + exp2f(player_move.z));

            glm::vec3 player_move_norm = player_move / norm_fac;

            if (will_sprint) {
                player_move_norm = player_move_norm * SPRINTING;
            }
            glm::vec3 world_move = glm::mat3(view_mat) * player_move_norm;

            physics->body_->setLinearVelocity(btVector3(world_move[0], oldY, world_move[2]));

            /*
            switch (player->state_) {
                case STATE_STANDING: {
                    if (will_sprint) {
                        player_move_norm = player_move_norm * SPRINTING;
                    }
                    glm::vec3 world_move = glm::mat3(view_mat) * player_move_norm;

                    physics->body_->setLinearVelocity(btVector3(world_move[0], oldY, world_move[2]));
                    break;
                }
                case STATE_AIRBOURNE: {
                    glm::vec3 world_move = glm::mat3(view_mat) * player_move_norm;

                    physics->body_->setLinearVelocity(btVector3(world_move[0], oldY, world_move[2]));
                    break;
                }
                default:
                    break;
            }
             */


            will_move_ = false;
            /*
            MyMotionState *motionState = physics->body_->getMotionState();
            motionState->

            transform->local_ = transform->local_ * glm::yawPitchRoll(-player->yaw_, 0.0f, 0.0f) * transl * glm::yawPitchRoll(player->yaw_, 0.0f, 0.0f);
            transform->is_dirty_ = true;
             */

        } else /*if (is_on_ground) */ {
            physics->body_->setLinearVelocity(btVector3(0.0f, oldY, 0.0f));
        }


        if (player->state_ == STATE_AIRBOURNE) {
            will_jump_ = false;
        }
        else if (will_jump_ && player->state_ == STATE_STANDING) {
            std::cout << "Will jump" << std::endl;
            physics->body_->setLinearVelocity(btVector3(0.0f, 3.0f, 0.0f));
            will_jump_ = false;
        }

        /* DIMENSION CHANGE */
        if (dim_change_in_progress_) {
            dim_change_accumulator_ += dt;

            // If dimChange completed, set state to not changing
            if (dim_change_accumulator_ > TOTAL_DIM_CHANGE_DURATION) {
                dim_change_in_progress_ = false;
            }

            else {
                events.emit<DimensionChangeInProgressEvent>(
                        dim_change_accumulator_ / TOTAL_DIM_CHANGE_DURATION, dim_from_, dim_change_counter_);
            }

            if (dim_change_accumulator_ >= TOTAL_DIM_CHANGE_DURATION / 2 && last_dim_change_ != dim_change_counter_) {
                Dim &d = dimension->dimension_;
                d = (d == DIMENSION_ONE) ? DIMENSION_TWO : DIMENSION_ONE;
                last_dim_change_ = dim_change_counter_;
            }
        }
    }
}

void sw::PlayerControlSystem::receive(const StartDimensionChangeEvent &startDimChange) {
    // Don't change until last dimChange is completed
    if (!dim_change_in_progress_) {
        dim_change_in_progress_ = true;
        dim_change_accumulator_ = 0.0;
        dim_from_ = current_dim_;
        dim_change_counter_++;
    }

}

void sw::PlayerControlSystem::receive(const DimensionChangeInProgressEvent &dimChanged) { }

void sw::PlayerControlSystem::receive(const JumpEvent &jump) {
    will_jump_ = true;
}

void sw::PlayerControlSystem::receive(const MovementEvent &move) {

    move_forward_ = move.forward_;
    move_right_ = move.right_;
    will_sprint = move.is_sprinting_;
    will_move_ = true;
}

void sw::PlayerControlSystem::receive(const ViewChangedEvent &view) {
    delta_yaw_ = view.delta_right_ * ANGLE_SCALE_FACTOR;

    pitch_ += view.delta_up_ * ANGLE_SCALE_FACTOR;
    if (pitch_ >= PITCH_MAX) {
        pitch_ = PITCH_MAX;
    } else if (pitch_ <= PITCH_MIN) {
        pitch_ = PITCH_MIN;
    }

    will_change_view_ = true;
}




