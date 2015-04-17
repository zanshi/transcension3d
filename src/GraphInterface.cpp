//
// Created by Benjamin Wiberg on 15-04-17.
//

#pragma once

#include "GraphInterface.hpp"

#include "components/GraphNodeComponent.hpp"

namespace sw {
    namespace GraphInterface {
        void addChild(ex::Entity parent, ex::Entity child_new) {
            auto parent_node = parent.component<GraphNodeComponent>();
            auto child_node = child.component<GraphNodeComponent>();

            // Check that both parent and child has a GraphNodeComponent
            if (!parent_node || !child_node)
                return;

            // Check so that the child isn't already a child of the new parent
            std::vector<ex::Entity> &c = parent_node->children_;
            if (std::find(c.begin(), c.end(), child_new) != c.end())
                return;

            // Remove the child from its old parent, set its new parent, and add the child to the parent vector
            removeItself(child_new);
            child_node->parent_ = parent;
            c.push_back(child_new);
        }

        void removeChildren(ex::Entity parent) {
            auto node = parent.component<GraphNodeComponent>();
            if (!node)
                return;

            std::vector<ex::Entity> &c = node->children_;

            std::for_each(c.begin(), c.end(), [&] (ex::Entity child) {
                auto child_node = parent.component<GraphNodeComponent>();
                if (child_node) {
                    child_node->parent_ = null;
                }
            });
            parent->children_.clear();
        }

        void removeChild(ex::Entity parent, ex::Entity child_to_remove) {

        }

        void removeItself(ex::Entity entity) {
            auto node = entity.component<GraphNodeComponent>();
            if (!node)
                return;

            // Get the parent's GraphNodeComponent and make sure that the parent actually has one
            auto parent_node = node->parent_.component<GraphNodeComponent>();
            assert(parent_node);

            // Remove itself from the parent's vector of children
            std::vector<ex::Entity> &c = parent_node->children_;
            c.erase( std::remove( std::begin(c), std::end(c), entity), std::end(c) );
        }
    }
}