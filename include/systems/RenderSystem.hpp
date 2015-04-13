//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "entityx/entityx.h"

#include "glm/glm.hpp"

#include "components/TransformComponent.hpp"
#include "components/RenderComponent.hpp"
#include "components/GraphNodeComponent.hpp"
#include "components/MovementComponent.hpp"

#include "events/RenderEvent.hpp"

#include "game_constants.hpp"

namespace ex = entityx;

namespace sw {

/** Placeholder rendering system that processes entities with the following components
 *      - TransformComponent
 *      - RenderComponent
 *
 *  It logs the BodyComponent's position and rotation vectors as well as the RenderComponent's debug string
 *  **/

    class RenderSystem : public ex::System<RenderSystem> {
    public:
        void setRootEntity(ex::Entity root) { root_ = root; };

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
            events_ = events;

            // Calculate the interpolation factor alpha
            float alpha = static_cast<float>(dt / TIME_STEP);

            // Start rendering at the top of the tree
            render(root_, false, alpha);
            /*
            ex::ComponentHandle <BodyComponent> body;
            ex::ComponentHandle <RenderComponent> render;

            for (ex::Entity entity : es.entities_with_components(body, render)) {
                // RENDER -> represented by emitting a RenderEvent, TODO: make it ACTUALLY render something...
                events.emit<RenderEvent>(entity);

            }
             */
        }

        void render(ex::Entity entityToRender, bool dirty, float alpha) {
            auto transform = entityToRender.component<TransformComponent>();
            auto render = entityToRender.component<RenderComponent>();
            auto graphNode = entityToRender.component<GraphNodeComponent>();
            auto movement = entityToRender.component<MovementComponent>();

            // See if we need to update the current entities cached world transform
            dirty |= transform->is_dirty;
            if (dirty) {
                // If dirty, update its cached world transform
                combine(transform, graphNode->parent_);
                transform->is_dirty = false;
            }

            // Render if the current entity has a RenderComponent
            if (render) {
                // The current entity can be rendered, so render it ffs
                // RENDER -> represented by emitting a RenderEvent, TODO: make it ACTUALLY render something...
                events.emit<RenderEvent>(entityToRender);
            }

            // Render its children
            for (ex::Entity child : graphNode->children_) {
                render(child, dirty, alpha);
            }
        }

    private:
        // Ugly-ass hack for events
        ex::EventManager &events_;
        // Reference to the top node in the tree, where rendering ALWAYS starts
        ex::Entity root_;

        void combine(ex::ComponentHandle<TransformComponent> transform, ex::Entity parent_entity) {
            glm::mat4 &local = transform->local_;

            // Make sure the parent has a TransformComponent
            // TODO: Fix entity dependencies so all entities have Transform- and GraphNodeComponents
            auto transform_parent = parent_entity.component<TransformComponent>();
            assert(transform_parent);

            glm::mat4 &parent_world = transform_parent->cached_world_;

            // Multiply the local transform with the parent's world transform
            transform->cached_world_ = parent_world * local;
        }

    };
}