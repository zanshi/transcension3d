//
// Created by Benjamin Wiberg on 15-05-05.
//


#pragma once

#include <events/AudioEvent.hpp>
#include "entityx/entityx.h"
#include "SDL.h"

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/QuitEvent.hpp"
#include "events/ViewChangedEvent.hpp"
#include "events/StartDimensionChangeEvent.hpp"
#include "events/DebugdrawerEvent.hpp"

namespace ex = entityx;

namespace sw {
    class InputSystem : public ex::System<InputSystem> {
    public:

        InputSystem() {
            isPlayingMusic = true;
        }

        ~InputSystem() {}

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            assert(window_w != -1 && window_h != -1 &&
                   "InputSystem::setWindowSize(w, h) must be called before first Appl. update");

            mouse_xrel_ = mouse_yrel_ = 0;

            SDL_Event e;

            const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

            forward = right = 0;
            is_sprinting = false;


            if (currentKeyStates[SDL_SCANCODE_W]) {
                forward += 1;
            }
            if (currentKeyStates[SDL_SCANCODE_A]) {
                right += -1;
            }
            if (currentKeyStates[SDL_SCANCODE_S]) {
                forward += -1;
            }
            if (currentKeyStates[SDL_SCANCODE_D]) {
                right += 1;
            }
            if (currentKeyStates[SDL_SCANCODE_LSHIFT]) {
                is_sprinting = true;
            }

            if (forward != 0 || right != 0)
                events.emit<MovementEvent>(right, forward, is_sprinting);

            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    events.emit<QuitEvent>();
                }

                if (e.type == SDL_KEYDOWN) {
                    SDL_bool isRelMouseMode = SDL_GetRelativeMouseMode();
                    int isMouseVisible = SDL_ShowCursor(SDL_QUERY);

                    switch (e.key.keysym.sym) {
                        case SDLK_SPACE:
                            events.emit<JumpEvent>();
                            break;
                        case SDLK_m:
                            if (isRelMouseMode == SDL_TRUE)
                                SDL_SetRelativeMouseMode(SDL_FALSE);
                            else
                                SDL_SetRelativeMouseMode(SDL_TRUE);

                            if (isMouseVisible == SDL_ENABLE)
                                SDL_ShowCursor(SDL_DISABLE);
                            else
                                SDL_ShowCursor(SDL_ENABLE);
                            break;
                        case SDLK_p:
                            events.emit<StartDimensionChangeEvent>();
                            break;
                        case SDLK_o:
                            debug_draw = true;
                            events.emit<DebugdrawerEvent>(debug_draw);
                            break;
                        case SDLK_i:
                            debug_draw = false;
                            events.emit<DebugdrawerEvent>(debug_draw);
                            break;
                        case SDLK_u:
                            events.emit<AudioEvent>(isPlayingMusic);
                            isPlayingMusic = !isPlayingMusic;
                            break;
                        default:
                            break;
                    }
                }

                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    switch (e.button.button) {
                        case SDL_BUTTON_LEFT:
                            std::cout << "pew" << std::endl;
                            break;
                        default:
                            break;
                    }
                }
                if (e.type == SDL_MOUSEMOTION) {
                    mouse_xrel_ = e.motion.xrel;
                    mouse_yrel_ = e.motion.yrel;
                }

                // Window state handling
                if (e.type == SDL_WINDOWEVENT) {
                    switch (e.window.event) {
                        case SDL_WINDOWEVENT_RESIZED: {
                            window_w = e.window.data1;
                            window_h = e.window.data2;
                        };
                            break;

                        default:
                            break;
                    }
                }
            }

            // If mouse moved
            if (mouse_xrel_ != 0 || mouse_yrel_ != 0) {
                events.emit<ViewChangedEvent>((float)mouse_xrel_ / (float)window_w,
                                              (float)mouse_yrel_ / (float)window_w);
            }
        }

        void setWindowSize(const int w, const int h) {
            window_w = w;
            window_h = h;
        }

    private:
        float right, forward;
        int mouse_xrel_, mouse_yrel_;
        int window_w = -1, window_h = -1;

        bool is_sprinting;
        bool isPlayingMusic;

        bool debug_draw;
    };
}
