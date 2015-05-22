//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include "entityx/entityx.h"

#include "btBulletDynamicsCommon.h"

#include "components/PhysicsComponent.hpp"

#include "physics/MyDebugDrawer.hpp"

#include "systems/PlayerControlSystem.hpp"

namespace ex = entityx;

namespace sw {

    class PhysicsSystem : public ex::System<PhysicsSystem>/*, public ex::Receiver<PhysicsSystem>*/ {
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

            m_pWorld->setGravity(btVector3(0, -10, 0));

            debugDrawer_ = new MyDebugDrawer();
            m_pWorld->setDebugDrawer(debugDrawer_);

            m_pWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);

        }


        void configure(ex::EventManager &events) override {
            //events.subscribe<DimensionChangeInProgressEvent>(*this);
        }


        void populateWorld(ex::EntityManager &es) {
            auto physics = ex::ComponentHandle<PhysicsComponent>();
            for (ex::Entity e : es.entities_with_components(physics)) {
                std::cout << "populateworld" << std::endl;
                std::cout << "Group: " << physics->group_ << std::endl;
                std::cout << "Mask: " << physics->mask_ << std::endl;
                std::cout << "Previous flags: " << physics->body_->getCollisionFlags() << std::endl;
                //m_pWorld->addRigidBody(physics->body_, physics->group_, physics->mask_);
                physics->body_->setActivationState(DISABLE_DEACTIVATION);
                m_pWorld->addRigidBody(physics->body_);
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


        void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) {
            if (m_pWorld) {
                m_pWorld->stepSimulation(dt);
                //m_pWorld->debugDrawWorld();
                //debugDrawer_->drawLines();

            }
        }

/*
        void receive(const DimensionChangeInProgressEvent &dimChange) {
            if (dimChange.completion_factor_ >= 0.5f) {
                // The dimension change has occurred
                auto dimension = ex::ComponentHandle<DimensionComponent>();
                auto physics = ex::ComponentHandle<PhysicsComponent>();

                for (ex::Entity e : entityManager.entities_with_components(dimension, physics)) {
                    if (dimension->dimension_ == Dim::DIMENSION_BOTH)
                        continue;

                    if (dimension->dimension_ == Dim::DIMENSION_ONE) {

                    }

                    m_pWorld->add


                }
            }
        }
        */


    private:
        btBroadphaseInterface *m_pBroadphase;
        btCollisionDispatcher *m_pDispatcher;
        btSequentialImpulseConstraintSolver *m_pSolver;
        btDefaultCollisionConfiguration *m_pCollisionConfiguration;

        btDynamicsWorld *m_pWorld;

        MyDebugDrawer *debugDrawer_;


    };
}