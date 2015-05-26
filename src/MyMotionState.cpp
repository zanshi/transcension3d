//
// Created by Niclas Olmenius on 2015-05-26.
//


#include "physics/MyMotionState.hpp"


namespace sw {

    sw::MyMotionState::MyMotionState(ex::Entity entity) : entity_(entity), transformComponent_(entity.component<TransformComponent>()){

        std::cout << "transformComponent: " << glm::to_string(transformComponent_->cached_world_) << std::endl;

        transform_.setFromOpenGLMatrix(glm::value_ptr(transformComponent_->cached_world_));

        glm::mat4 test;

        transform_.getOpenGLMatrix(glm::value_ptr(test));

        // std::cout << "transform: " << glm::to_string(test) << std::endl;
    }

    sw::MyMotionState::~MyMotionState() {
    }


    void sw::MyMotionState::getWorldTransform(btTransform &worldTrans) const {
        worldTrans = transform_;
    }

    void sw::MyMotionState::setWorldTransform(const btTransform &worldTrans) {
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


}