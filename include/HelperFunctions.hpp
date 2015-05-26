//
// Created by Niclas Olmenius on 2015-05-22.
//

#pragma once

#include "btBulletDynamicsCommon.h"
#include "glm/glm.hpp"
#include <iomanip>

namespace sw {

    void printMatVec(const btVector3 &vec);

    void printMatVec(const glm::vec3 &vec);

    void printMatVec(const glm::mat4 &mat);

    void printMatVec(const btTransform &trans);

    template<class T>
    void printMatVec(std::string name, const T &type) {
        std::cout << typeid(T).name() << " name: " << name << std::endl;
        printMatVec(std::move(type));
        std::cout << std::endl;
    }

    btVector3 vec3_to_btVector3(glm::vec3 vec);

    glm::vec3 btVector3_to_vec3(btVector3 vec);


    /* GLM - DEBUGGING FUNCTIONS */

    /* print glm matrix */
    void print_glmMatrix(glm::mat4 pMat4);

    /* print glm vector */
    void print_glmVec3(glm::vec3 vec);

}