//
// Created by Benjamin Wiberg on 15-05-05.
//


#pragma once

#include "entityx/entityx.h"
#include "SDL.h"

namespace ex = entityx;

namespace sw {
    class InputSystem : public ex::System<InputSystem> {
    public:
        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            SDL_Event e;

            while (SDL_PollEvent(&e)) {

                switch (e.type) {
                    case SDL_QUIT:
                        events.emit<QuitEvent>();
                        std::cout << "Quit pls" << std::endl;
                        break;
                        /* Look for a keypress */
                    case SDL_KEYDOWN:
                        /* Check the SDLKey values */
                        switch( e.key.keysym.sym ){
                            case SDLK_ESCAPE:
                                events.emit<QuitEvent>();
                                std::cout << "Quit pls" << std::endl;
                                break;
                            default:
                                break;
                        }
                    default:
                        break;
                }
            }
        }
    };
}