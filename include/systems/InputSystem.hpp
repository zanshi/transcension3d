//
// Created by Benjamin Wiberg on 15-05-05.
//


#pragma once

#include <events/AudioEvent.hpp>
#include "entityx/entityx.h"


#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/QuitEvent.hpp"
#include "events/ViewChangedEvent.hpp"
#include "events/StartDimensionChangeEvent.hpp"
#include "events/PickUpObjectEvent.hpp"
#include "events/DebugdrawerEvent.hpp"
#include "events/SpacePressedEvent.hpp"

namespace ex = entityx;

namespace sw {
    class InputSystem : public ex::System<InputSystem> {
    public:

        InputSystem();

        ~InputSystem() { }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

        void setWindowSize(const int w, const int h);

    private:
        float right, forward;
        int mouse_xrel_, mouse_yrel_;
        int window_w = -1, window_h = -1;

        bool is_sprinting;
        bool isPlayingMusic;

        bool debug_draw;
    };
}
