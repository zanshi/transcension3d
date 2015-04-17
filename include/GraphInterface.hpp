//
// Created by Benjamin Wiberg on 15-04-17.
//

#pragma once

#include <entityx/entityx.h>

namespace ex = entityx;

namespace sw {
    /** GraphInterface **/
    // The interface gives functions to manipulate the tree graph
    namespace GraphInterface {
        // Makes sure that the parent has EXACTLY ONE reference to the child
        void addChild(ex::Entity parent, ex::Entity child_new);

        // Remove all the parent's children
        void removeChildren(ex::Entity parent);

        // Remove a specific child, if it has it
        void removeChild(ex::Entity parent, ex::Entity child_to_remove);

        // Detach the entity from the tree graph.
        // After the function the specified entity has an empty parent,
        // and its last parent has removed its reference to it
        void removeItself(ex::Entity entity);
    }
}