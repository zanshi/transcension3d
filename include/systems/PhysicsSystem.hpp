//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include "entityx/entityx.h"

#include "btBulletDynamicsCommon.h"

#include "components/PhysicsComponent.hpp"

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
            m_pWorld->setGravity(btVector3(0, -9.81f, 0));
        }


        void configure(ex::EventManager &events) override {

        }


        void populateWorld(ex::Entity current_entity) {

            auto physicsComponent = current_entity.component<PhysicsComponent>();
            auto node = current_entity.component<GraphNodeComponent>();

            if(physicsComponent != nullptr) {
                m_pWorld->addRigidBody(physicsComponent->body);
            }

            if(!node->children_.empty()) {
                for(ex::Entity child : node->children_) {
                    populateWorld(child);
                }
            }

        }



        ~PhysicsSystem() {
            delete m_pBroadphase;
            delete m_pDispatcher;
            delete m_pCollisionConfiguration;
            delete m_pWorld;
            delete m_pSolver;
        }


        void update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt) {
            if (m_pWorld) {
                m_pWorld->stepSimulation(dt);
            }
        }



        private:

        btBroadphaseInterface *m_pBroadphase;

        btCollisionDispatcher *m_pDispatcher;

        btSequentialImpulseConstraintSolver *m_pSolver;

        btDefaultCollisionConfiguration *m_pCollisionConfiguration;

        btDynamicsWorld *m_pWorld;


    };
}