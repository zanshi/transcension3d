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

        MyMotionState(ex::Entity entity) : entity_(entity), transformComponent_(entity.component<TransformComponent>()){

            std::cout << "transformComponent: " << glm::to_string(transformComponent_->cached_world_) << std::endl;

            transform_.setFromOpenGLMatrix(glm::value_ptr(transformComponent_->cached_world_));

            glm::mat4 test;

            transform_.getOpenGLMatrix(glm::value_ptr(test));

            //std::cout << "transform: " << glm::to_string(test) << std::endl;
        }

        virtual ~MyMotionState() {
        }


        virtual void getWorldTransform(btTransform &worldTrans) const {
            worldTrans = transform_;
        }

        virtual void setWorldTransform(const btTransform &worldTrans) {
            if (!transformComponent_) { return; }

            transform_ = worldTrans;

            /*
            glm::mat4 temp;

            worldTrans.getOpenGLMatrix(glm::value_ptr(temp));

            //std::cout << "transform: " << glm::to_string(temp) << std::endl;


            btQuaternion rot = worldTrans.getRotation();
            btVector3 pos = worldTrans.getOrigin();


            recalculate_world_transform(entity_, rot, pos);

            */
        }

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