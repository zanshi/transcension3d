//
// Created by Niclas Olmenius on 2015-05-06.
//

#pragma once

#include "btBulletDynamicsCommon.h"
#include "entityx/entityx.h"
#include <glm/gtc/type_ptr.hpp>

namespace ex = entityx;

namespace sw {


    class MyMotionState : public btMotionState {

        public:

        MyMotionState(ex::Entity entity) : entity_(entity)  {

            auto transform = entity.component<TransformComponent>();

            btTransform initialTransform;
            initialTransform.setFromOpenGLMatrix(glm::value_ptr(transform->cached_world_));

            mInitialPosition_ = initialTransform;

        }

        virtual ~MyMotionState() {
        }


        virtual void getWorldTransform(btTransform &worldTrans) const
        {
            worldTrans = mInitialPosition_;
        }

        virtual void setWorldTransform(const btTransform &worldTrans)
        {

            std::cout << "setworldtransform" << std::endl;
            auto transformComponent = entity_.component<TransformComponent>();

            if(!transformComponent) { return; }

//            btQuaternion rot = worldTrans.getRotation();
//
//            transformComponent->orientation_.w = rot.w();
//            transformComponent->orientation_.x = rot.x();
//            transformComponent->orientation_.y = rot.y();
//            transformComponent->orientation_.z = rot.z();
//
//            btVector3 pos = worldTrans.getOrigin();
//
//            transformComponent->position_.x = pos.x();
//            transformComponent->position_.y = pos.y();
//            transformComponent->position_.z = pos.z();
//
//            transformComponent->update_local_transform();
//
//            transformComponent->is_dirty_ = true;

            worldTrans.getOpenGLMatrix(glm::value_ptr(transformComponent->cached_world_));


        }

        protected:

        btTransform mInitialPosition_;
        ex::Entity entity_;



    };

}