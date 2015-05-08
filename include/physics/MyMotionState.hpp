//
// Created by Niclas Olmenius on 2015-05-06.
//

#pragma once

#include "btBulletDynamicsCommon.h"
#include "entityx/entityx.h"

namespace ex = entityx;

namespace sw {


    class MyMotionState : public btMotionState {

        public:

        MyMotionState(ex::Entity entity) : entity_(entity)  {

            auto transform = entity.component<TransformComponent>();

            btQuaternion initQuat(transform->orientation_.x,transform->orientation_.y
            ,transform->orientation_.z, transform->orientation_.w);
            btVector3 initPos(transform->position_.x, transform->position_.y, transform->position_.z);

            btTransform initialTransform(initQuat, initPos);

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

            auto transformComponent = entity_.component<TransformComponent>();

            if(!transformComponent) { return; }

            btQuaternion rot = worldTrans.getRotation();

            transformComponent->orientation_.w = rot.w();
            transformComponent->orientation_.x = rot.x();
            transformComponent->orientation_.y = rot.y();
            transformComponent->orientation_.z = rot.z();

            btVector3 pos = worldTrans.getOrigin();

            transformComponent->position_.x = pos.x();
            transformComponent->position_.y = pos.y();
            transformComponent->position_.z = pos.z();

        }

        protected:

        btTransform mInitialPosition_;
        ex::Entity entity_;



    };

}