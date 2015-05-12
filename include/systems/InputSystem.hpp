//
// Created by Benjamin Wiberg on 15-05-05.
//


#pragma once

#include "entityx/entityx.h"
#include "SDL.h"

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/QuitEvent.hpp"
#include "events/ViewChangedEvent.hpp"
#include "events/DimensionChangedEvent.hpp"

namespace ex = entityx;

namespace sw {
    class InputSystem : public ex::System<InputSystem> {
    public:
        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
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
                            events.emit<DimensionChangedEvent>();
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
                    events.emit<ViewChangedEvent>(e.motion.xrel, e.motion.yrel);
                }
            }
        }

    private:
        float right, forward;
        bool is_sprinting;
    };
}
