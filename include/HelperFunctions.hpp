//
// Created by Niclas Olmenius on 2015-05-22.
//

#pragma once

#include "btBulletDynamicsCommon.h"
#include "glm/glm.hpp"
#include <iomanip>

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

    template<class T>
    void printMatVec(std::string name, const T &type) {
        std::cout << typeid(T).name() << " name: " << name << std::endl;
        printMatVec(std::move(type));
        std::cout << std::endl;
    }

    /* GLM - DEBUGGING FUNCTIONS */

    /* print glm matrix */
    void print_glmMatrix(glm::mat4 pMat4) {

        double dArray[16] = {0.0};
        const float *pSource = (const float *) glm::value_ptr(glm::transpose(pMat4));
        for (int i = 0; i < 16; ++i) {
            dArray[i] = pSource[i];
            std::cout << std::fixed << std::setprecision(2);
            std::cout << dArray[i] << " ";
            if (i == 3 || i == 7 || i == 11)
                std::cout << std::endl;
        }
        std::cout << std::endl << "--------------------" << std::endl;
    }

    /* print glm vector */
    void print_glmVec3(glm::vec3 vec) {
        for (int i = 0; i < 3; ++i) {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << vec[i] << " ";
        }
        std::cout << std::endl << "--------------------" << std::endl;
    }

}