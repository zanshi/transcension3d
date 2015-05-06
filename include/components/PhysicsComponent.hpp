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

        PhysicsComponent(glm::vec3 boundingVector, float mass = 0.0f)
                : motionState_(new MyMotionState(btTransform::getIdentity())),
                  shape_(new btBoxShape(btVector3(boundingVector.x, boundingVector.y, boundingVector.z))) {


            btRigidBody::btRigidBodyConstructionInfo bodyConstructionInfo(mass, motionState_.get(), shape_.get());

            body_ = new btRigidBody(bodyConstructionInfo);


        }


        std::unique_ptr<MyMotionState> motionState_;

        std::unique_ptr<btCollisionShape> shape_;


        btRigidBody *body_;


    };


}