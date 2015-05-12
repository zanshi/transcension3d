//
// Created by Cristoffer Särevall on 2015-05-06.
//

#pragma once

#include "entityx/entityx.h"

#include "components/PlayerComponent.hpp"

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/ViewChangedEvent.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"


namespace ex = entityx;

namespace sw {
    class PlayerControlSystem : public ex::System<PlayerControlSystem>, public ex::Receiver<PlayerControlSystem> {

    public:
        void configure(ex::EventManager &events) override {
            events.subscribe<JumpEvent>(*this);
            events.subscribe<MovementEvent>(*this);
            events.subscribe<ViewChangedEvent>(*this);
        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            auto player = ex::ComponentHandle<PlayerComponent>();
            auto transform = ex::ComponentHandle<TransformComponent>();
            auto physics = ex::ComponentHandle<PhysicsComponent>();

            for (ex::Entity entity : es.entities_with_components(player, transform, physics)) {
                //Update the player

                /* Rotation */
                if (will_change_view_) {
                    // X-rotation a.k.a. yaw
                    player->yaw_ += delta_yaw_;

                    // Y-rotation a.k.a. pitch, "looking up or down"
                    player->pitch_ = pitch_;

                    will_change_view_ = false;
                }

                /* Movement */
                if (will_move_) {
                    glm::mat4 transl = glm::translate(glm::mat4(1), {move_right_*MOVE_SCALE_FACTOR, 0.0f, -move_forward_*MOVE_SCALE_FACTOR});

                    transform->local_ = transform->local_ * glm::yawPitchRoll(-player->yaw_, 0.0f, 0.0f) * transl * glm::yawPitchRoll(player->yaw_, 0.0f, 0.0f);

                    will_move_ = false;
                }

            }
        }

        void receive(const JumpEvent &jump) {

        }

        void receive(const MovementEvent &move) {
            const float SPRINTING = 1.5f;
            move_forward_ = move.forward_ + move.is_sprinting_*move.forward_*SPRINTING;
            move_right_ = move.right_ + move.is_sprinting_*move.right_*SPRINTING;
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
        bool will_move_ = false, will_change_view_ = false;
        float move_forward_, move_right_;
        float delta_yaw_, pitch_;

        // ROTATION MATRIX CONSTANTS
        const float PITCH_MAX = (float) M_PI / 2, PITCH_MIN = -(float) M_PI / 2;
        const float ANGLE_SCALE_FACTOR = 0.0025f;
        const float MOVE_SCALE_FACTOR = 0.01f;
    };
}
