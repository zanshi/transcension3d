//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include "entityx/entityx.h"

#include "btBulletDynamicsCommon.h"

#include "components/PhysicsComponent.hpp"

#include "physics/MyDebugDrawer.hpp"

namespace ex = entityx;

namespace sw {

    class PhysicsSystem : public ex::System<PhysicsSystem> {
        public:

        PhysicsSystem() {
            // create the collision configuration
            m_pCollisionConfiguration = new btDefaultCollisionConfiguration();

            // Default collision dispatcher
            m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

            // create the broadphase
            m_pBroadphase = new btDbvtBroadphase();

            // create the constraint solver
            m_pSolver = new btSequentialImpulseConstraintSolver();


            // create the world
            m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

            m_pWorld->setGravity(btVector3(0, -1, 0));


            debugDrawer_ = new MyDebugDrawer();
            //m_pWorld->setDebugDrawer(debugDrawer_);

            //m_pWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawAabb);




        }


        void configure(ex::EventManager &events) override {

        }


        void populateWorld(ex::Entity current_entity) {



            auto physicsComponent = current_entity.component<PhysicsComponent>();
            auto node = current_entity.component<GraphNodeComponent>();

            if(physicsComponent) {
                std::cout << "populateworld" << std::endl;
                std::cout << "Group: " << physicsComponent->group_ << std::endl;
                std::cout << "Mask: " << physicsComponent->mask_ << std::endl;
                std::cout << "Previous flags: " << physicsComponent->body_->getCollisionFlags() << std::endl;
                //m_pWorld->addRigidBody(physicsComponent->body_, physicsComponent->group_, physicsComponent->mask_);
                physicsComponent->body_->setActivationState(DISABLE_DEACTIVATION);
                m_pWorld->addRigidBody(physicsComponent->body_);
            }

            if(!node->children_.empty()) {
                for(ex::Entity child : node->children_) {
                    populateWorld(child);
                }
            }

        }



        ~PhysicsSystem() {
            delete m_pWorld;
            delete debugDrawer_;
            delete m_pBroadphase;
            delete m_pDispatcher;
            delete m_pCollisionConfiguration;
            delete m_pSolver;
        }


        void update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt) {
            if (m_pWorld) {
                m_pWorld->stepSimulation(dt);

            }

//            for(int i = m_pWorld->getNumCollisionObjects()-1; i>=0; i--) {
//
//                btCollisionObject *obj = m_pWorld->getCollisionObjectArray()[i];
//                btRigidBody *body = btRigidBody::upcast(obj);
//                btTransform trans;
//
//                if (body && body->getMotionState()) {
//                    body->getMotionState()->getWorldTransform(trans);
//                } else {
//                    trans = obj->getWorldTransform();
//                }
//
//
//                std::cout << "world pos obj: " << i << ", " << static_cast<float>(trans.getOrigin().getX())
//                        << ", " << static_cast<float>(trans.getOrigin().getY()) << ", " <<
//                        static_cast<float>(trans.getOrigin().getZ()) << std::endl;
//
//            }



        }



        private:

        btBroadphaseInterface *m_pBroadphase;

        btCollisionDispatcher *m_pDispatcher;

        btSequentialImpulseConstraintSolver *m_pSolver;

        btDefaultCollisionConfiguration *m_pCollisionConfiguration;

        btDynamicsWorld *m_pWorld;

        MyDebugDrawer* debugDrawer_;


    };
}