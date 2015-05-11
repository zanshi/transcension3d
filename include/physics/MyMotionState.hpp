//
// Created by Niclas Olmenius on 2015-05-06.
//

#pragma once

#include "btBulletDynamicsCommon.h"
#include "entityx/entityx.h"
#include "components/TransformComponent.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

namespace ex = entityx;

namespace sw {


    class MyMotionState : public btMotionState {

        public:

        MyMotionState(ex::ComponentHandle<TransformComponent> transform)
                : transformComponent_(transform)
        {

            std::cout << "transformComponent: " << glm::to_string(transformComponent_->cached_world_) << std::endl;

            transform_.setFromOpenGLMatrix(glm::value_ptr(transformComponent_->cached_world_));

            glm::mat4 test;

            transform_.getOpenGLMatrix(glm::value_ptr(test));

            std::cout << "transform: " << glm::to_string(test) << std::endl;
        }

        virtual ~MyMotionState() {
        }


        virtual void getWorldTransform(btTransform &worldTrans) const
        {
            std::cout << "getworldtransform" << std::endl;

            worldTrans = transform_;
        }

        virtual void setWorldTransform(const btTransform &worldTrans)
        {

            //std::cout << "setworldtransform" << std::endl;

            if(!transformComponent_) { return; }

            transform_ = worldTrans;

            glm::mat4 temp;

            worldTrans.getOpenGLMatrix(glm::value_ptr(temp));

            //std::cout << "transform: " << glm::to_string(temp) << std::endl;

            transformComponent_->cached_world_ = temp;


        }

        protected:

        btTransform transform_;
        ex::ComponentHandle<TransformComponent> transformComponent_;



    };

}