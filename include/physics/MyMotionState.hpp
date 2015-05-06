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

        MyMotionState(const btTransform &initialPosition)
        {
            mInitialPosition = initialPosition;
        }

        virtual ~MyMotionState() {
        }


        virtual void getWorldTransform(btTransform &worldTrans) const
        {
            worldTrans = mInitialPosition;
        }

        virtual void setWorldTransform(const btTransform &worldTrans)
        {

//        btQuaternion rot = worldTrans.getRotation();
//        mSceneNode ->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
//        btVector3 pos = worldTrans.getOrigin();
//        mSceneNode ->setPosition(pos.x(), pos.y(), pos.z());
        }

        protected:

        btTransform mInitialPosition;


    };

}