//
// Created by Benjamin Wiberg on 15-05-05.
//


#pragma once

#include "entityx/entityx.h"
#include "SDL.h"

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/QuitEvent.hpp"
#include "events/ViewEvent.hpp"

namespace ex = entityx;

namespace sw {
    class InputSystem : public ex::System<InputSystem> {
    public:
        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            SDL_Event e;

            const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );


            if( currentKeyStates[ SDL_SCANCODE_W ] )
            {
                forward += 1;
            }
            if( currentKeyStates[ SDL_SCANCODE_A ] )
            {
                right += -1;
            }
            if( currentKeyStates[ SDL_SCANCODE_S ] )
            {
                forward += -1;
            }
            if( currentKeyStates[ SDL_SCANCODE_D ] )
            {
                right += 1;
            }
            events.emit<MovementEvent>(right, forward);

            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    events.emit<QuitEvent>();
                }

                if(e.type == SDL_KEYDOWN) {

                    switch(e.key.keysym.sym) {

                        case SDLK_SPACE:
                            events.emit<JumpEvent>();
                            break;

                        default:
                            break;
                    }
                }


                if(e.type == SDL_MOUSEBUTTONDOWN)  {

                    switch(e.button.button) {
                        case SDL_BUTTON_LEFT:
                            std::cout << "pew" << std::endl;
                            break;
                        default:
                            break;
                    }
                }
                if(e.type == SDL_MOUSEMOTION) {

                    events.emit<ViewEvent>();

                }
            }
        }
    private:
        int right, forward;
    };
}
