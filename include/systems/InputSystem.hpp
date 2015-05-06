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

            x = 0;
            y = 0;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    events.emit<QuitEvent>();
                }

                const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );


                if( currentKeyStates[ SDL_SCANCODE_W ] )
                {
                    std::cout << "heloo" << std::endl;
                }
                if( currentKeyStates[ SDL_SCANCODE_A ] )
                {
                    std::cout << "heloo" << std::endl;
                }
                if( currentKeyStates[ SDL_SCANCODE_S ] )
                {
                    std::cout << "heloo" << std::endl;
                }
                if( currentKeyStates[ SDL_SCANCODE_D ] )
                {
                    std::cout << "heloo" << std::endl;
                }
                if( currentKeyStates[ SDL_SCANCODE_LCTRL ] )
                {
                    std::cout << "heloo" << std::endl;
                }

                if(e.type == SDL_KEYDOWN) {

                    switch(e.key.keysym.sym) {

                        case SDLK_w:
                            y += 1;
                            break;

                        case SDLK_s:
                            y += -1;
                            break;

                        case SDLK_a:
                            x += -1;
                            break;

                        case SDLK_d:
                            x += 1;
                            break;

                        case SDLK_SPACE:
                            events.emit<JumpEvent>();
                            break;

                        default:
                            break;
                    }
                    events.emit<MovementEvent>(x, y);
                }


                if(e.type == SDL_MOUSEBUTTONDOWN)  {

                    switch(e.button.button) {
                        case SDL_BUTTON_LEFT:
                            std::cout << "pew" << std::endl;
                            break;
                        default:
                            std::cout << "hmm" << std::endl;
                            break;
                    }
                }
                if(e.type == SDL_MOUSEMOTION) {

                    events.emit<ViewEvent>();

                }
            }
        }
    private:
        int x,y;
    };
}
