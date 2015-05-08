//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include "btBulletDynamicsCommon.h"
#include "components/TransformComponent.hpp"
#include "physics/MyMotionState.hpp"

namespace sw {

    struct PhysicsComponent : public ex::Component<PhysicsComponent> {


        // Entity - PhysicsComponent -> btRigidBody
        //              PhysicsSystem  -^

        // When removing entity, call PhysicsSystem->m_pWorld->removebtRigidbody(?) with
        // the PhysicsComponent->body as argument

        PhysicsComponent() {}

        PhysicsComponent(ex::Entity entity, glm::vec3 boundingVector, float mass = 0.0f ) {
            motionState_ = new MyMotionState(entity);
            shape_ = new btBoxShape(btVector3(boundingVector.x, boundingVector.y, boundingVector.z));

            // calculate the local inertia
            btVector3 localInertia(0, 0, 0);

            // objects of infinite mass can't
            // move or rotate
            if (mass != 0.0f)
                shape_->calculateLocalInertia(mass, localInertia);


            btRigidBody::btRigidBodyConstructionInfo bodyConstructionInfo(mass, motionState_, shape_,localInertia);

            body_ = new btRigidBody(bodyConstructionInfo);

            std::cout << "boundingvector xyz: " << boundingVector.x << " " << boundingVector.y << " "
            << boundingVector.z << std::endl;


        }


        MyMotionState *motionState_;

        btCollisionShape *shape_;

        btRigidBody *body_;


    };


}