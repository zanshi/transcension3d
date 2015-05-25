//
// Created by Cristoffer Särevall on 2015-05-06.
//

#pragma once

#include "entityx/entityx.h"

#include "components/PlayerComponent.hpp"
#include "components/DimensionComponent.hpp"

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/ViewChangedEvent.hpp"
#include "events/DimensionChangeInProgressEvent.hpp"
#include "events/StartDimensionChangeEvent.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"


namespace ex = entityx;

namespace sw {
    class PlayerControlSystem : public ex::System<PlayerControlSystem>, public ex::Receiver<PlayerControlSystem> {

    public:
        PlayerControlSystem() {
            current_dim_ = sw::STARTING_DIMENSION;
            dim_from_ = current_dim_;
            dim_change_counter_ = 0;
            last_dim_change_ = -1;
        }

        void configure(ex::EventManager &events) override {
            events.subscribe<JumpEvent>(*this);
            events.subscribe<MovementEvent>(*this);
            events.subscribe<ViewChangedEvent>(*this);
            events.subscribe<DimensionChangeInProgressEvent>(*this);
            events.subscribe<StartDimensionChangeEvent>(*this);
        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
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
                            move_forward_ * glm::vec3(0.0f, 0.0f, - 1.0f) + move_right_ * glm::vec3(1.0f, 0.0f, 0.0f);

                    glm::vec3 world_move = glm::mat3(view_mat) * player_move;

                    physics->body_->setLinearVelocity(btVector3(world_move[0], oldY, world_move[2]));

                    /*
                    MyMotionState *motionState = physics->body_->getMotionState();
                    motionState->

                    transform->local_ = transform->local_ * glm::yawPitchRoll(-player->yaw_, 0.0f, 0.0f) * transl * glm::yawPitchRoll(player->yaw_, 0.0f, 0.0f);
                    transform->is_dirty_ = true;
                     */
                    will_move_ = false;
                }  else /*if (is_on_ground) */ {
                    physics->body_->setLinearVelocity(btVector3(0.0f, oldY, 0.0f));
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

                    if (dim_change_accumulator_ >= TOTAL_DIM_CHANGE_DURATION/2 && last_dim_change_ != dim_change_counter_) {
                        Dim &d = dimension->dimension_;
                        d = (d == DIMENSION_ONE) ? DIMENSION_TWO : DIMENSION_ONE;
                        last_dim_change_ = dim_change_counter_;
                    }
                }
            }
        }

        void receive(const StartDimensionChangeEvent &startDimChange) {
            // Don't change until last dimChange is completed
            if (!dim_change_in_progress_) {
                dim_change_in_progress_ = true;
                dim_change_accumulator_ = 0.0;
                dim_from_ = current_dim_;
                dim_change_counter_++;
            }

        }

        void receive(const DimensionChangeInProgressEvent &dimChanged) { }

        void receive(const JumpEvent &jump) { }

        void receive(const MovementEvent &move) {
            const float SPRINTING = 1.5f;
            move_forward_ = move.forward_ + move.is_sprinting_ * move.forward_ * SPRINTING;
            move_right_ = move.right_ + move.is_sprinting_ * move.right_ * SPRINTING;
            will_move_ = true;
        }

        void receive(const ViewChangedEvent &view) {
            delta_yaw_ = view.delta_right_ * ANGLE_SCALE_FACTOR;

            pitch_ += view.delta_up_ * ANGLE_SCALE_FACTOR;
            if (pitch_ >= PITCH_MAX) {
                pitch_ = PITCH_MAX;
            } else if (pitch_ <= PITCH_MIN) {
                pitch_ = PITCH_MIN;
            }

            will_change_view_ = true;
        }

    private:
        Dim current_dim_, dim_from_;

        bool will_move_ = false, will_change_view_ = false;
        float move_forward_, move_right_;
        float delta_yaw_, pitch_;

        // ROTATION MATRIX CONSTANTS
        const float PITCH_MAX = (float) M_PI / 2, PITCH_MIN = -(float) M_PI / 2;
        const float ANGLE_SCALE_FACTOR = 2.f;
        const float MOVE_SCALE_FACTOR = 1.f;

        // Dimension Change shizniz
        bool dim_change_in_progress_ = false;

        const ex::TimeDelta TOTAL_DIM_CHANGE_DURATION = 1.0;
        ex::TimeDelta dim_change_accumulator_;

        int dim_change_counter_;
        int last_dim_change_;
    };

}
