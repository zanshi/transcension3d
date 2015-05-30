//
// Created by Cristoffer Särevall on 2015-05-06.
//

#pragma once

#include "entityx/entityx.h"

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/ViewChangedEvent.hpp"
#include "events/DimensionChangeInProgressEvent.hpp"
#include "events/StartDimensionChangeEvent.hpp"


namespace ex = entityx;

namespace sw {
    class PlayerControlSystem : public ex::System<PlayerControlSystem>, public ex::Receiver<PlayerControlSystem> {

    public:
        PlayerControlSystem();

        void configure(ex::EventManager &events) override;

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

        void receive(const StartDimensionChangeEvent &startDimChange);

        void receive(const DimensionChangeInProgressEvent &dimChanged);

        void receive(const JumpEvent &jump);

        void receive(const MovementEvent &move);

        void receive(const ViewChangedEvent &view);

    private:
        Dim current_dim_, dim_from_;

        bool will_move_ = false, will_change_view_ = false;
        bool will_jump_ = false, will_sprint = false;
        float move_forward_, move_right_;
        float delta_yaw_, pitch_;

        // ROTATION MATRIX CONSTANTS
        const float PITCH_MAX = (float) M_PI / 2, PITCH_MIN = -(float) M_PI / 2;
        const float ANGLE_SCALE_FACTOR = 2.f;
        const float MOVE_SCALE_FACTOR = 1.f;
        const float SPRINTING = 2.5f;

        // Dimension Change shizniz
        bool dim_change_in_progress_ = false;

        const ex::TimeDelta TOTAL_DIM_CHANGE_DURATION = 1.0;
        ex::TimeDelta dim_change_accumulator_;

        int dim_change_counter_;
        int last_dim_change_;
    };

}
