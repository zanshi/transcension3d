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
                if(e.type == SDL_KEYDOWN) {

                    switch(e.key.keysym.sym) {

                        case SDLK_w:

                            for (ex::Entity entity : es.entities_with_components<PhysicsComponent>()) {

                                //std::cout << "hej" << std::endl;

                                auto physics = entity.component<PhysicsComponent>();

                                physics->body_->setLinearVelocity(btVector3(0.0f, 1.0f, 0.0f));


                                //physics->body_->setWorldTransform()
                            }

                            //y += 1;
                            break;

                        case SDLK_s:
                            y += -1;
                            //std::cout << "ner" << std::endl;
                            break;

                        case SDLK_a:
                            x += -1;
                            //std::cout << "vänster" << std::endl;
                            break;

                        case SDLK_d:
                            x += 1;
                            //std::cout << "höger" << std::endl;
                            break;

                        case SDLK_SPACE:
                            events.emit<JumpEvent>();
                            //std::cout << "hopp" << std::endl;
                            break;

                        default:
                            std::cout << "hmm" << std::endl;
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
                            std::cout << "kuk" << std::endl;
                            break;
                    }
                }
                if(e.type == SDL_MOUSEMOTION) {

                    //std::cout << "x:" << e.motion.xrel << std::endl << "y:" << e.motion.yrel << std::endl;

                }
            }
        }
    private:
        int x,y;
    };
}
