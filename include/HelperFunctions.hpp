//
// Created by Niclas Olmenius on 2015-05-22.
//



#pragma once

#include "btBulletDynamicsCommon.h"
#include "glm/glm.hpp"

namespace sw {

    void printMatVec(const btVector3 &vec) {
        std::cout << " x = " << vec.x() << "  y = " << vec.y() << "  z = " << vec.z() << std::endl;
    }

    void printMatVec(const glm::vec3 &vec) {
        std::cout << " x = " << vec.x << "  y = " << vec.y << "  z = " << vec.z << std::endl;
    }

    void printMatVec(const glm::mat4 &mat) {
        std::cout << glm::to_string(mat) << std::endl;
    }

    void printMatVec(const btTransform &trans) {
        glm::mat4 temp;
        trans.getOpenGLMatrix(glm::value_ptr(temp));
        printMatVec(temp);
    }

    template <class T>
    void printMatVec(std::string name, const T& type) {
        std::cout << typeid(T).name() << " name: " << name << std::endl;
        printMatVec(std::move(type));
        std::cout << std::endl;
    }


}