//
// Created by Benjamin Wiberg on 2015-04-13.
//


#pragma once

#include <vector>

#include "entityx/entityx.h"

namespace ex = entityx;

namespace sw {

    struct GraphNodeComponent : public ex::Component<GraphNodeComponent> {

        GraphNodeComponent() { }

        GraphNodeComponent(ex::Entity parent, ex::Entity itself) {
            // Make sure the specified parent isn't the entity itself
            auto parentNode = parent.component<GraphNodeComponent>();
            if (parentNode) {
                assert(parentNode.get() != this);
                // Add 'itself' to the parent's children.
                // There is no chance in hell that it already is in the vector, so we don't bother with that.
                parentNode->children_.push_back(itself);
            } else {
                // 'itself' is the root entity, whose "parent" is an empty entity,
                // thus, we should NOT add 'itself' as a child to the parent
                // Do nothing...
            }
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