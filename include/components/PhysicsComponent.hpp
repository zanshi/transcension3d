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

        PhysicsComponent() { }

        ~PhysicsComponent() {
            delete motionState_;
            delete shape_;
            delete body_;
        }

        PhysicsComponent(ex::Entity entity, btCollisionShape *collisionShape,
                         btScalar mass = 0.0f, short group = sw::COL_DYNAMIC, short mask = sw::COL_STATIC)
                : group_(group), mask_(mask) {

            motionState_ = new MyMotionState(entity);
            shape_ = collisionShape;

            std::cout << "PhysicsComponent" << std::endl;
            std::cout << "LocalScaling: " << (*shape_->getLocalScaling()) << std::endl;
            std::cout << "getShapeType: " << (shape_->getShapeType()) << std::endl;
            std::cout << "getName     : " << (shape_->getName()) << std::endl;
            std::cout << "isNonMoving : " << (shape_->isNonMoving()) << std::endl;
            std::cout << "getMargin   : " << (shape_->getMargin()) << std::endl;

            //shape_ = new btBoxShape(btVector3(2.0f, 1.0f, 1.0f));

            // calculate the local inertia
            btVector3 localInertia(0, 0, 0);

            // objects of infinite mass can't
            // move or rotate



            if (mass != 0.0f)
                shape_->calculateLocalInertia(mass, localInertia);


            btRigidBody::btRigidBodyConstructionInfo bodyConstructionInfo(mass, motionState_, shape_, localInertia);

            body_ = new btRigidBody(bodyConstructionInfo);


//            if ( mass != 0.0f) {
//                body_->setCollisionFlags(body_->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
//                body_->setActivationState(DISABLE_DEACTIVATION);
//            }



        }


        short group_;
        short mask_;

        MyMotionState *motionState_;

        btCollisionShape *shape_;

        btRigidBody *body_;


    };


}