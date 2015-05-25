//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include "entityx/entityx.h"

#include "btBulletDynamicsCommon.h"

#include "components/PhysicsComponent.hpp"

#include "physics/MyDebugDrawer.hpp"

#include "systems/PlayerControlSystem.hpp"

#include "HelperFunctions.hpp"

namespace ex = entityx;

namespace sw {

    class PhysicsSystem : public ex::System<PhysicsSystem>, public ex::Receiver<PhysicsSystem> {
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

            will_update_current_bodies_ = false;
            dim_from_ = Dim::DIMENSION_ONE;
            last_dim_change_num = -1;
        }


        void configure(ex::EventManager &events) override {
            events.subscribe<DimensionChangeInProgressEvent>(*this);
        }


        void populateWorld(ex::EntityManager &es) {
            auto physics = ex::ComponentHandle<PhysicsComponent>();
            auto dimension = ex::ComponentHandle<DimensionComponent>();

            for (ex::Entity e : es.entities_with_components(physics, dimension)) {
                Dim d = dimension->dimension_;

                std::cout << "populateworld" << std::endl;
                /*
                std::cout << "Group: " << physics->group_ << std::endl;
                std::cout << "Mask: " << physics->mask_ << std::endl;
                std::cout << "Previous flags: " << physics->body_->getCollisionFlags() << std::endl;
                 */
                std::cout << "  dimension: " << d << "\n";
                physics->body_->activate(true);

                m_pWorld->addRigidBody(physics->body_, d, d);
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
            if (will_update_current_bodies_) {
                /*std::cout << "Will update bitches\n";
                auto dimension = ex::ComponentHandle<DimensionComponent>();
                auto physics = ex::ComponentHandle<PhysicsComponent>();

                for (ex::Entity e : entityManager.entities_with_components(dimension, physics)) {
                    Dim d = dimension->dimension_;

                    if (d == Dim::DIMENSION_BOTH)
                        continue;

                    if (d == dim_from_) {
                        //m_pWorld->removeRigidBody(physics->body_);
                        physics->body_->setActivationState(ISLAND_SLEEPING);
                        //physics->body_->setActivationState(DISABLE_SIMULATION);
                        std::cout << "Disabled a bitch\n";
                    }

                    else {
                        //m_pWorld->addRigidBody(physics->body_);
                        //physics->body_->setActivationState(DISABLE_DEACTIVATION);
                        physics->body_->activate(true);
                        std::cout << "Enabled a bitch\n";
                    }
                }*/
                auto physics = ex::ComponentHandle<PhysicsComponent>();
                auto dimension = ex::ComponentHandle<DimensionComponent>();
                auto player = ex::ComponentHandle<PlayerComponent>();

                for (ex::Entity p : entityManager.entities_with_components(player, physics, dimension)) {
                    Dim d = dimension->dimension_;
                    m_pWorld->removeRigidBody(physics->body_);
                    m_pWorld->addRigidBody(physics->body_, d, d);
                    std::cout << "Current player dimension is: " << d << "\n";
                }

                will_update_current_bodies_ = false;
            }

            m_pWorld->stepSimulation(dt);
            //m_pWorld->debugDrawWorld();
            //debugDrawer_->drawLines();

//            auto player = ex::ComponentHandle<PlayerComponent>();
//            auto physics = ex::ComponentHandle<PhysicsComponent>();
//
//            for(ex::Entity e : entityManager.entities_with_components(player, physics)) {
//
//                btTransform temp;
//                btVector3 tempVec;
//                btVector3 playerMin;
//
//                physics->body_->getCollisionShape()->getAabb(physics->body_->getWorldTransform(), playerMin, tempVec);
//
//                //sw::printMatVec("PlayerMin", playerMin);
//
//                btVector3 btTo(playerMin.x(), playerMin.y(), playerMin.z()-5000.0f);
//
//                btCollisionWorld::ClosestRayResultCallback res(playerMin, btTo);
//
//                m_pWorld->rayTest(playerMin, btTo, res);
//
//                res.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
//
//
//                if(res.hasHit()) {
//
//                    btTo = res.m_hitPointWorld;
//                    btVector3 normal = res.m_hitNormalWorld;
//
//                    btVector3 p = playerMin.lerp(btTo,res.m_closestHitFraction);
//
//                    sw::printMatVec("Collision, p: ", p);
//
//
//
//
//                }
//
//            }



        }

        void receive(const DimensionChangeInProgressEvent &dimChange) {
            if (last_dim_change_num == dimChange.NUM_)
                return;

            if (dimChange.completion_factor_ >= 0.5f) {
                dim_from_ = dimChange.dim_from_;
                last_dim_change_num = dimChange.NUM_;

                // The dimension change has occurred
                will_update_current_bodies_ = true;
            }
        }

    private:
        btBroadphaseInterface *m_pBroadphase;

        btCollisionDispatcher *m_pDispatcher;
        btSequentialImpulseConstraintSolver *m_pSolver;
        btDefaultCollisionConfiguration *m_pCollisionConfiguration;
        btDynamicsWorld *m_pWorld;

        MyDebugDrawer *debugDrawer_;

        /* DIMENSION CHANGE */
        Dim dim_from_;
        bool will_update_current_bodies_;
        int last_dim_change_num;
    };
}