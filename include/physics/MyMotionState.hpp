//
// Created by Niclas Olmenius on 2015-05-06.
//

#pragma once

#include "btBulletDynamicsCommon.h"
#include "entityx/entityx.h"
#include "components/TransformComponent.hpp"
#include "components/GraphNodeComponent.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace ex = entityx;

namespace sw {


    class MyMotionState : public btMotionState {

    public:

        MyMotionState(ex::Entity entity);

        virtual ~MyMotionState();

        virtual void getWorldTransform(btTransform &worldTrans) const override;

        virtual void setWorldTransform(const btTransform &worldTrans) override;

    protected:

        btTransform transform_;
        ex::Entity entity_;
        ex::ComponentHandle<TransformComponent> transformComponent_;

    private:
        /*
        void recalculate_local_transform(glm::mat4 current_transform) {

            glm::mat4 change = current_transform *
                               glm::affineInverse(node_->parent_.component<TransformComponent>()->cached_world_);

        }

        void recalculate_world_transform(ex::Entity entity, btQuaternion rot, btVector3 pos) {

            auto transform = entity.component<TransformComponent>();

            transform->world_rotation_ = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());
            transform->world_position_ = glm::vec3(pos.x(), pos.y(), pos.z());
            transform->update_world_transform();

            if (!node_->children_.empty()) {
                for (ex::Entity e : node->children_) {
                    recalculate_world_transform(e, rot, pos);
                }
            }


        }
         */


    };

}