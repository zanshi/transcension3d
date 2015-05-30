//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

// Include GLEW
#include "GL/glew.h"
#include "SDL.h"
#include "entityx/entityx.h"
#include "events/QuitEvent.hpp"

namespace ex = entityx;

namespace sw {

    class Application : public ex::EntityX, public ex::Receiver<Application> {
    public:
        explicit Application();

        void update(ex::TimeDelta dt);

        bool init();

        void run(std::string input);

        void receive(const QuitEvent &quitEvent);

    private:
        SDL_Window *win;
        SDL_GLContext glcontext;

        bool isRunning;

        ex::Entity root_;
        unsigned int current_dim_;

        void initScene(std::string input);

        // Utilities
        void updateFPS(float newFPS);
    };
}