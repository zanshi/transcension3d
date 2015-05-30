//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>
#include <events/DebugdrawerEvent.hpp>
#include <events/GravityChangeEvent.hpp>
#include "entityx/entityx.h"
#include "btBulletDynamicsCommon.h"
#include "components/PhysicsComponent.hpp"
#include "physics/MyDebugDrawer.hpp"
#include "HelperFunctions.hpp"

#include "events/ProjectionViewEvent.hpp"
#include "events/PickUpObjectEvent.hpp"

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
            events.subscribe<PickUpObjectEvent>(*this);
            events.subscribe<ProjectionViewEvent>(*this);
            events.subscribe<GravityChangeEvent>(*this);

        }


        void populateWorld(ex::EntityManager &es) {
            auto physics = ex::ComponentHandle<PhysicsComponent>();
            auto dimension = ex::ComponentHandle<DimensionComponent>();

            for (ex::Entity e : es.entities_with_components(physics, dimension)) {
                Dim d = dimension->dimension_;

                std::cout << "populateworld" << std::endl;
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

            if (m_debugMode) {
                m_pWorld->debugDrawWorld();
                debugDrawer_->drawLines();

            }


            auto player = ex::ComponentHandle<PlayerComponent>();
            auto physics = ex::ComponentHandle<PhysicsComponent>();

            for (ex::Entity e : entityManager.entities_with_components(player, physics)) {

                if (!proj_view_events_.empty()) {
                    view_ = proj_view_events_.front().view_matrix_;
                    camera_projection_ = proj_view_events_.front().projection_matrix_;

                    proj_view_events_.clear();
                }

                if (!pickup_events_.empty()) {
                    processPickupEvents(physics);
                }

                btTransform temp;
                btVector3 tempVec;
                btVector3 playerMin;

                physics->body_->getMotionState()->getWorldTransform(temp);

                playerMin = temp.getOrigin();

                playerMin.setY(playerMin.getY() - (physics->height_));

                btVector3 btTo(playerMin.x(), -500.0f, playerMin.z());

                btCollisionWorld::ClosestRayResultCallback res(playerMin, btTo);

                m_pWorld->rayTest(playerMin, btTo, res);

                res.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

                res.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;


                if (res.hasHit()) {

                    btTo = res.m_hitPointWorld;
                    btVector3 normal = res.m_hitNormalWorld;

                    btScalar p = playerMin.distance(btTo);

                    //sw::printMatVec("Collision, p: ", p);

                    //sw::printMatVec("Playermin: ", playerMin);

                    //std::cout << "Distance to collision point: " << p << std::endl;

                    //std::cout << "p: " << p << " ,distance_to_bottom_: " << player->distance_to_bottom_ << std::endl;

                    if (p > 0.05 && std::abs(physics->body_->getLinearVelocity().getY()) > 0.0) {
                        player->state_ = STATE_AIRBOURNE;
                    } else {
                        player->state_ = STATE_STANDING;
                    }
                }
            }

            //std::cout << "Player speed: " << physics->body_->getLinearVelocity().getY() << std::endl;
        }

        // Process pickup events generated this frame
        void processPickupEvents(ex::ComponentHandle<PhysicsComponent> physics) {

            pickup_events_.clear();

            glm::mat4 inverted_view = glm::inverse(view_);

            glm::vec4 lookAtInViewSpace(0, 0, -1,
                                        1); // Forward vector (use 0, 0, -1 if you have a right-handed coordinate system)
            glm::vec4 lookAtInWorldSpaceTemp = lookAtInViewSpace * inverted_view; // Transform into world space.

            glm::vec3 lookAtInWorldSpace = glm::vec3(lookAtInWorldSpaceTemp);

            lookAtInWorldSpace = lookAtInWorldSpace * 1000.0f;

            // The player's position
            btTransform temp;
            physics->body_->getMotionState()->getWorldTransform(temp);
            btVector3 bt_out_origin = temp.getOrigin();

            // The ray's direction
            btVector3 bt_out_direction = sw::vec3_to_btVector3(lookAtInWorldSpace);

            printMatVec("View: ", view_);
            printMatVec("Origin: ", bt_out_origin);
            printMatVec("Direction", bt_out_direction);

            btCollisionWorld::ClosestRayResultCallback RayCallback(bt_out_origin, bt_out_direction);
            RayCallback.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
            RayCallback.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;

            m_pWorld->rayTest(bt_out_origin, bt_out_direction, RayCallback);

            std::string message;

            // If the ray hit something, set the object's linear velocity and print its position
            if (RayCallback.hasHit()) {
                btRigidBody *temp_body = (btRigidBody *) RayCallback.m_collisionObject->getUserPointer();
                int colFlag = temp_body->getCollisionFlags();
                int angFac = temp_body->getAngularFactor().getX();
                if(!is_lifting && colFlag != 1 && angFac != 0.f) {
                    std::ostringstream oss;
                    oss << "hit";
                    btTransform transA, transB;
                    bool useLinearReferenceFrameA = true;
                    transA.setIdentity();
                    transB.setIdentity();
                    bt_out_direction.setY(0.5f);
                    transB.setOrigin(bt_out_direction / 1000.f);
                    cons = new btGeneric6DofConstraint(*temp_body, *physics->body_, transA,
                                                                                transB, useLinearReferenceFrameA);
                    m_pWorld->addConstraint(cons);
                    temp_body->setAngularFactor(0.f);
                    temp_body->setActivationState(DISABLE_DEACTIVATION);
                    is_lifting = true;
                    return;
                }
            } else {
                message = "background";
            }
            if (is_lifting) {
                cons->getRigidBodyA().setAngularFactor(1.f);
                cons->getRigidBodyA().setActivationState(ACTIVE_TAG);
                cons->getRigidBodyA().removeConstraintRef(cons);
                cons->getRigidBodyB().removeConstraintRef(cons);
                m_pWorld->removeConstraint(cons);
                delete cons;
                is_lifting = false;
            }

            std::cout << message << std::endl;


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

        void receive(const ProjectionViewEvent &proj_view_event) {
            proj_view_events_.push_back(proj_view_event);
        }


        void receive(const PickUpObjectEvent &pickUpObjectEvent) {
            pickup_events_.push_back(pickUpObjectEvent);
        }

        void receive(const GravityChangeEvent &gravityChangeEvent){
            if (gravityChangeEvent.gravityChange_){
                for (int n=0;n<m_pWorld->getCollisionObjectArray().size();n++) {
                    m_pWorld->getCollisionObjectArray().at(n)->setActivationState(DISABLE_DEACTIVATION);
                }
                    m_pWorld->setGravity(btVector3(0.f,0.1f,0.f));
            }
            else{
                for (int n=0;n<m_pWorld->getCollisionObjectArray().size();n++) {
                    m_pWorld->getCollisionObjectArray().at(n)->setActivationState(ACTIVE_TAG);
                }
                m_pWorld->setGravity(btVector3(0.f,-10.,0.f));
            }
        }

    private:
        btBroadphaseInterface *m_pBroadphase;

        btCollisionDispatcher *m_pDispatcher;
        btSequentialImpulseConstraintSolver *m_pSolver;
        btDefaultCollisionConfiguration *m_pCollisionConfiguration;
        btDynamicsWorld *m_pWorld;

        MyDebugDrawer *debugDrawer_;

        std::vector<PickUpObjectEvent> pickup_events_;
        std::vector<ProjectionViewEvent> proj_view_events_;

        glm::mat4 camera_projection_;
        glm::mat4 view_;


        bool m_debugMode = false;

        bool is_lifting = false;
        btGeneric6DofConstraint *cons;


        /* DIMENSION CHANGE */
        Dim dim_from_;
        bool will_update_current_bodies_;
        int last_dim_change_num;


    };

}
