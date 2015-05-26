//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <events/DebugdrawerEvent.hpp>
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
            events.subscribe<DebugdrawerEvent>(*this);
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

            auto player = ex::ComponentHandle<PlayerComponent>();
            for (ex::Entity e : es.entities_with_components(player, physics)) {

                btTransform temp;
                physics->body_->getMotionState()->getWorldTransform(temp);
                btVector3 playerMin = temp.getOrigin();
                btVector3 btTo(playerMin.x(), -500.0f, playerMin.z());

                btCollisionWorld::ClosestRayResultCallback res(playerMin, btTo);

                m_pWorld->rayTest(playerMin, btTo, res);

                if (res.hasHit()) {


                    btTo = res.m_hitPointWorld;
                    btScalar p = playerMin.distance(btTo);

                    player->distance_to_bottom_ = physics->height_;

                    std::cout << "Distance to bottom: " << p << std::endl;


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

            if(m_debugMode){
                m_pWorld->debugDrawWorld();
                debugDrawer_->drawLines();

            }


            auto player = ex::ComponentHandle<PlayerComponent>();
            auto physics = ex::ComponentHandle<PhysicsComponent>();

            for(ex::Entity e : entityManager.entities_with_components(player, physics)) {

                btTransform temp;
                btVector3 tempVec;
                btVector3 playerMin;

                physics->body_->getMotionState()->getWorldTransform(temp);

                playerMin = temp.getOrigin();

                playerMin.setY(playerMin.getY()-(physics->height_));

                //std::cout << "playerMin: " << playerMin.getY() << std::endl;

                btVector3 btTo(playerMin.x(), -500.0f, playerMin.z());

                btCollisionWorld::ClosestRayResultCallback res(playerMin, btTo);

                //btCollisionWorld::ClosestConvexResultCallback

                m_pWorld->rayTest(playerMin, btTo, res);

                res.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

                res.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;


                if(res.hasHit()) {

                    btTo = res.m_hitPointWorld;
                    btVector3 normal = res.m_hitNormalWorld;

                    btScalar p = playerMin.distance(btTo);

                    //sw::printMatVec("Collision, p: ", p);

                    //sw::printMatVec("Playermin: ", playerMin);

                    //std::cout << "Distance to collision point: " << p << std::endl;

                    //std::cout << "p: " << p << " ,distance_to_bottom_: " << player->distance_to_bottom_ << std::endl;

                    if(p > 0.05 && std::abs(physics->body_->getLinearVelocity().getY()) > 0.0) {
                        player->is_on_ground_ = false;
                    } else {
                        player->is_on_ground_ = true;
                    }

                    //std::cout << "Player speed: " << physics->body_->getLinearVelocity().getY() << std::endl;
                }

            }



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


        void receive(const DebugdrawerEvent &debugdraw) {
            m_debugMode = debugdraw.debugMode_;
        }

        private:
        btBroadphaseInterface *m_pBroadphase;

        btCollisionDispatcher *m_pDispatcher;
        btSequentialImpulseConstraintSolver *m_pSolver;
        btDefaultCollisionConfiguration *m_pCollisionConfiguration;
        btDynamicsWorld *m_pWorld;

        MyDebugDrawer *debugDrawer_;


        bool m_debugMode = false;



        /* DIMENSION CHANGE */
            Dim dim_from_;
            bool will_update_current_bodies_;
            int last_dim_change_num;


        };

    }