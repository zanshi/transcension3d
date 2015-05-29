//
// Created by Benjamin Wiberg on 15-05-05.
//


#pragma once

#include <events/AudioEvent.hpp>
#include "entityx/entityx.h"
#include "SDL.h"
#include <math.h>       /* fmax */
#include <cmath>

#include "events/JumpEvent.hpp"
#include "events/MovementEvent.hpp"
#include "events/QuitEvent.hpp"
#include "events/ViewChangedEvent.hpp"
#include "events/StartDimensionChangeEvent.hpp"
#include "events/PickUpObjectEvent.hpp"
#include "events/DebugdrawerEvent.hpp"

namespace ex = entityx;

namespace sw {
    class InputSystem : public ex::System<InputSystem> {
    public:

        InputSystem() {
            isPlayingMusic = true;
        }

        ~InputSystem() { }

        void AddController(int id) {
            if (SDL_IsGameController(id)) {
                pad = SDL_GameControllerOpen(id);

                if (pad) {
                    joy = SDL_GameControllerGetJoystick(pad);
                    int instanceID = SDL_JoystickInstanceID(joy);

                    // You can add to your own map of joystick IDs to controllers here.
                    //YOUR_FUNCTION_THAT_CREATES_A_MAPPING( id, pad );
                }
            }
        }

        void RemoveController(int id) {
            //pad = YOUR_FUNCTION_THAT_RETRIEVES_A_MAPPING( id );
            SDL_GameControllerClose(pad);
        }

/*
        void OnControllerButton(const SDL_ControllerButtonEvent sdlEvent) {
            // Button presses and axis movements both sent here as SDL_ControllerButtonEvent structures

            switch (sdlEvent.button) {

                case SDL_CONTROLLER_BUTTON_A:
                    //events.emit<JumpEvent>();
                    break;
                case SDL_CONTROLLER_BUTTON_B:
                    break;
                case SDL_CONTROLLER_BUTTON_X:
                    break;
                case SDL_CONTROLLER_BUTTON_Y:
                    break;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    //is_sprinting = true;
                    break;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    break;
                case SDL_CONTROLLER_BUTTON_BACK:
                    break;
                case SDL_CONTROLLER_BUTTON_GUIDE:
                    break;
                case SDL_CONTROLLER_BUTTON_START:
                    break;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                    break;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    break;
                    //case SDL_CONTROLLER_BUTTON_INVALID:
                    //    break;
                default:
                    break;
            }
        }
*/
        void OnControllerAxis(const SDL_ControllerAxisEvent sdlEvent) {
            // Axis movements and button presses both sent here as SDL_ControllerAxisEvent structures

            /*-----------------------------------*/
            // sdlEvent.axis
            // 0 --> left analog --> left or right
            // 1 --> left analog --> up or down
            // 2 --> right analog --> left or right
            // 3 --> right analog --> up or down
            /*-----------------------------------*/

            const float MAX_VALUE_CONTROLLER = 32768;

            // x-axis for left analog joystick
            if (sdlEvent.axis == 0) {
                //right = sdlEvent.value / MAX_VALUE_CONTROLLER;


                // returns the normalized value between -1 and 1.
                right = fmaxf(-1.0f, (float) sdlEvent.value / MAX_VALUE_CONTROLLER);
                //std::cout << "--> right " << sdlEvent.value << std::endl;
            }
            // z-axis for left analog joystick
            if (sdlEvent.axis == 1) {
                //forward = sdlEvent.value / MAX_VALUE_CONTROLLER;


                // returns the normalized value between -1 and 1.
                forward = -fmaxf(-1.0f, (float) sdlEvent.value / MAX_VALUE_CONTROLLER);
                //std::cout << " forward: " << forward << std::endl;
            }

            // yaw --> right analog joystick
            if (sdlEvent.axis == 2) {


                //std::cout <<" sdlEvent.value: "<< sdlEvent.value << std::endl;

                if (abs(sdlEvent.value) > 500) {
                    mouse_xrel_ = 0.01 * fmaxf(-1.0f, (float) sdlEvent.value / MAX_VALUE_CONTROLLER);
                } else {
                    mouse_xrel_ = 0.0f;
                }
                /*
                if ( abs(sdlEvent.value) > 1000 ) {
                    mouse_xrel_ = sdlEvent.value;
                }
                 */
            }
            // pitch --> right analog joystick
            if (sdlEvent.axis == 3) {

                if (abs(sdlEvent.value) > 500) {
                    mouse_yrel_ = 0.01 * fmaxf(-1.0f, (float) sdlEvent.value / MAX_VALUE_CONTROLLER);
                } else {
                    mouse_yrel_ = 0.0f;
                }
                /*
                if ( abs(sdlEvent.value) > 1000 ) {
                    mouse_yrel_ = sdlEvent.value;
                }
                 */
            }


        }

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            assert(window_w != -1 && window_h != -1 &&
                   "InputSystem::setWindowSize(w, h) must be called before first Appl. update");

            //
            SDL_Event e;
            const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

            // Reset variables

            //forward = right = 0;
            //is_sprinting = false;


            // Keyboard
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


            /* OLD Place of emit movement event */

            //if (forward != 0 || right != 0)
            //    events.emit<MovementEvent>(right, forward, is_sprinting);

            while (SDL_PollEvent(&e)) {

                // Quit
                if (e.type == SDL_QUIT) {
                    events.emit<QuitEvent>();
                }

                // Game Controller
                switch (e.type) {
                    case SDL_CONTROLLERDEVICEADDED:
                        std::cout << "--> Game controller " << std::endl;
                        //int id = (int) e.cdevice;
                        AddController((int) e.cdevice.which);
                        break;
                    case SDL_CONTROLLERDEVICEREMOVED:
                        RemoveController((int) e.cdevice.which);
                        break;

                    case SDL_CONTROLLERBUTTONDOWN: {

                        switch (e.cbutton.button) {
                            case SDL_CONTROLLER_BUTTON_A:
                                events.emit<JumpEvent>();
                                break;
                            case SDL_CONTROLLER_BUTTON_Y:
                                events.emit<StartDimensionChangeEvent>();
                                break;
                            case SDL_CONTROLLER_BUTTON_B:
                                events.emit<PickUpObjectEvent>();
                                break;
                            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                                is_sprinting = true;
                                break;
                            case SDL_CONTROLLER_BUTTON_START:
                                events.emit<QuitEvent>();
                                break;
                            default:
                                break;
                        }

                        break;
                    }
                    case SDL_CONTROLLERBUTTONUP:
                        if(e.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
                            is_sprinting = false;
                        break;
                    case SDL_CONTROLLERAXISMOTION:
                        OnControllerAxis(e.caxis);
                        break;
                    default:
                        break;
                }

                /*
                // Mouse
                if (e.type == SDL_MOUSEMOTION) {
                    mouse_xrel_ = e.motion.xrel;
                    mouse_yrel_ = e.motion.yrel;
                }
                */


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
                        case SDLK_e:
                        case SDLK_f:
                            events.emit<PickUpObjectEvent>();
                            break;
                        case SDLK_u:
                            events.emit<AudioEvent>(isPlayingMusic);
                            isPlayingMusic = !isPlayingMusic;
                            break;
                        case SDLK_g:
                            events.emit<GravityChangeEvent>(gravityChange);
                            gravityChange = !gravityChange;
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

            //std::cout <<" mouse_xrel_: "<< mouse_xrel_ << " " <<  mouse_yrel_ << std::endl;
            //std::cout <<" ratio: "<< (float) mouse_xrel_ / (float) window_w << std::endl;
            //std::cout << " forward: " << forward << std::endl;




            if (fabs(forward) > 0.5f || fabs(right) > 0.5f) {
                //if (forward != 0 || right != 0)
                events.emit<MovementEvent>(right, forward, is_sprinting);
            }
            // If mouse moved
            if (fabs(mouse_xrel_) > 0.0002f || fabs(mouse_yrel_) > 0.0002f) {

                /*events.emit<ViewChangedEvent>( (float) mouse_xrel_ / (float) window_w,
                                              (float) mouse_yrel_ / (float) window_w);
                                              */
                //std::cout << "för i helvete" << std::endl;
                events.emit<ViewChangedEvent>(mouse_xrel_, mouse_yrel_);
            }
        }

        void setWindowSize(const int w, const int h) {
            window_w = w;
            window_h = h;
        }


    private:
        float right, forward;
        float mouse_xrel_, mouse_yrel_;
        int window_w = -1, window_h = -1;

        bool is_sprinting;
        bool isPlayingMusic;

        bool debug_draw;

        SDL_GameController *pad;
        SDL_Joystick *joy;

        bool gravityChange;

    };
}
