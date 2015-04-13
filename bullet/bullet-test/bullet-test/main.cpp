#include <iostream>
#include "btBulletDynamicsCommon.h"
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

int main() {
    
    // Build the broadphase
    // http://bulletphysics.org/mediawiki-1.5.8/index.php/Broadphase
    // http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    /* ...provides an interface to detect aabb-overlapping object pairs.
     
     Some implementations for this broadphase interface include btAxisSweep3, bt32BitAxisSweep3 and btDbvtBroadphase. The actual overlapping pair management, storage, adding and removing of pairs is dealt by the btOverlappingPairCache class.
     */
    
    
    
    // Set up the collision configuration and dispatcher
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    // ...allows to configure Bullet collision detection stack allocator, pool memory allocators
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    // ...supports algorithms that handle ConvexConvex and ConvexConcave collision pairs.
    // Time of Impact, Closest Points and Penetration Depth.
    
    
    
    // The actual physics solver
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    // ...is a fast SIMD implementation of the Projected Gauss Seidel (iterative LCP) method.
    
    
    
    // The world.
    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    // ...provides discrete rigid body simulation those classes replace the obsolete CcdPhysicsEnvironment/CcdPhysicsController
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
    
    
    
    // Create ground plane shape
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
    // ...provides an interface for collision shapes that can be shared among btCollisionObjects.
    
    
    
    // Create sphere shape
    btCollisionShape* fallShape = new btSphereShape(1);
    
    
    
    // Init ground
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    // ...provides a common implementation to synchronize world transforms with offsets.
    
    
    
    // Mass and inertia of ground (set to 0)
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    // ...structure provides information to create a rigid body.
    /* Setting mass to zero creates a fixed (non-dynamic) rigid body. For dynamic objects, you can use the collision shape to approximate the local inertia tensor, otherwise use the zero vector (default argument) You can use the motion state to synchronize the world transform between physics and graphics objects. And if the motion state is provided, the rigid body will initialize its initial world transform from the motion state, m_startWorldTransform is only used when you don't provide a motion state.
     */
    
    
    
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    /* ...is the main class for rigid body objects.
     
     It is derived from btCollisionObject, so it keeps a pointer to a btCollisionShape. It is recommended for performance and memory use to share btCollisionShape objects whenever possible. There are 3 types of rigid bodies:
     
     A) Dynamic rigid bodies, with positive mass. Motion is controlled by rigid body dynamics.
     B) Fixed objects with zero mass. They are not moving (basically collision objects)
     C) Kinematic objects, which are objects without mass, but the user can move them. There is on-way interaction, and Bullet calculates a velocity based on the timestep and previous and current world transform. Bullet automatically deactivates dynamic rigid bodies, when the velocity is below a threshold for a given time. Deactivated (sleeping) rigid bodies don't take any processing time, except a minor broadphase collision detection impact (to allow active objects to activate/wake up sleeping objects)
     */
    
    
    
    // Add ground to world
    dynamicsWorld->addRigidBody(groundRigidBody);
    
    
    // Place sphere 50m above the ground
    btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
    
    // Give it mass 1 kg and calculate inertia
    btScalar mass = 1;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    
    
    // Construct the rigid body and add it to the world
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
    dynamicsWorld->addRigidBody(fallRigidBody);
    
    
    for (int i = 0 ; i < 300 ; i++) {
        dynamicsWorld->stepSimulation(1 / 60.f, 10);
        
        btTransform trans;
        fallRigidBody->getMotionState()->getWorldTransform(trans);
        
        std::cout << "Sphere height: " << trans.getOrigin().getY() << std::endl;
    }
    
    
    // Clean up
    
    dynamicsWorld->removeRigidBody(fallRigidBody);
    delete fallRigidBody->getMotionState();
    delete fallRigidBody;
    dynamicsWorld->removeRigidBody(groundRigidBody);
    delete groundRigidBody->getMotionState();
    delete groundRigidBody;
    delete fallShape;
    delete groundShape;
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
    
    
    return 0;
}