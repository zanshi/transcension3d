//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "entityx/entityx.h"

namespace ex = entityx;

namespace sw {

    class Application : public ex::EntityX {
    public:
        explicit Application();

        void update(ex::TimeDelta dt);

        void initScene();

    private:
        ex::Entity root_;
        unsigned int current_dim_;

        // Setup function to initiate the RenderSystem with a root node
        void initSceneGraphRoot(ex::Entity root);
    };
}