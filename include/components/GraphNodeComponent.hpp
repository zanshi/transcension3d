//
// Created by Benjamin Wiberg on 2015-04-13.
//


#pragma once

#include <vector>
#include <entityx/entityx.h>

namespace ex = entityx;

namespace sw {

    struct GraphNodeComponent {
        GraphNodeComponent(ex::Entity parent) {
            // Make sure the parent isn't itself...
            auto parentNode = parent.component<GraphNodeComponent>();
            assert(parentNode && parentNode.get() != this);
            parent_ = parent;
        }

        ex::Entity parent_;
        std::vector<ex::Entity> children_;

        // Optional
        /*
        ex::ComponentHandle<GraphNodeComponent> getParentNode() {
            return parent_.component<GraphNodeComponent>();
        }

        std::vector<ex::ComponentHandle<GraphNodeComponent>> getChildrenNodes() {
            std::vector<ex::ComponentHandle<GraphNodeComponent> > vec;
            for (ex::Entity child : children_) {
                vec.push_back(child.component < GraphNodeComponent>());
            }
            return vec;
        }
        */
    };
}