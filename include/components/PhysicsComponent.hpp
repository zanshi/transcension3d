//
// Created by Adrian Palmquist on 15-04-28.
//

#pragma once

#include "btBulletDynamicsCommon.h"

namespace sw {

    struct PhysicsComponent : public ex::Component<PhysicsComponent> {


    // Entity - PhysicsComponent -> btRigidBody
    //              PhysicsSystem  -^

        // When removing entity, call PhysicsSystem->m_pWorld->removebtRigidbody(?) with
        // the PhysicsComponent->body as argument


    btRigidBody *body;


    };

}