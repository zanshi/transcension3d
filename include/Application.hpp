//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once


#include "SDL.h"

#undef main

#include "entityx/entityx.h"

#include "events/QuitEvent.hpp"

namespace ex = entityx;

namespace sw {

    class Application : public ex::EntityX, public ex::Receiver<Application> {
    public:
        explicit Application();

        void update(ex::TimeDelta dt);

        bool init();

        void run();

        void receive(const QuitEvent& quitEvent);

    private:
        SDL_Window *win;
        SDL_GLContext glcontext;

        bool isRunning;

        ex::Entity root_;
        unsigned int current_dim_;

        void initScene();

        // Setup function to initiate the RenderSystem with a root node
        void initSceneGraphRoot(ex::Entity root);
    };
}