#include "Constraint.h"

#include "./Body.h"

///////////////////////////////////////////////////////////////////////////////
// Mat6x6 with the all inverse mass and inverse I of bodies "a" and "b"
///////////////////////////////////////////////////////////////////////////////
//  [ 1/ma  0     0     0     0     0    ]
//  [ 0     1/ma  0     0     0     0    ]
//  [ 0     0     1/Ia  0     0     0    ]
//  [ 0     0     0     1/mb  0     0    ]
//  [ 0     0     0     0     1/mb  0    ]
//  [ 0     0     0     0     0     1/Ib ]
///////////////////////////////////////////////////////////////////////////////
MatMN Constraint::GetInvMassMatrix() const {
    MatMN result(6, 6);
    result.Zero();
    // a
    result.rows[0][0] = a->inverseMass;
    result.rows[1][1] = a->inverseMass;
    result.rows[2][2] = a->inverseMass;
    // b
    result.rows[3][3] = b->inverseMass;
    result.rows[4][4] = b->inverseMass;
    result.rows[5][5] = b->inverseMass;
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// VecN with the all linear and angular velocities of bodies "a" and "b"
///////////////////////////////////////////////////////////////////////////////
//  [ va.x ]
//  [ va.y ]
//  [ ωa   ]
//  [ vb.x ]
//  [ vb.y ]
//  [ ωb   ]
///////////////////////////////////////////////////////////////////////////////
VecN Constraint::GetVelocities() const {
    VecN V(6);
    V.Zero();
    // a
    V[0] = a->velocity.x;
    V[1] = a->velocity.y;
    V[2] = a->angularVelocity;
    // b
    V[3] = b->velocity.x;
    V[4] = b->velocity.y;
    V[5] = b->angularVelocity;
    return V;
}

///////////////////////////////////////////////////////////////////////////////
// JointConstraint
///////////////////////////////////////////////////////////////////////////////
JointConstraint::JointConstraint() : Constraint(), jacobian(1, 6) {}

JointConstraint::JointConstraint(Body *a, Body *b, const Vec2 &anchor) : Constraint(), jacobian(1, 6) {
    this->a = a;
    this->b = b;
    this->aPoint = a->GetLocalPoint(anchor);
    this->bPoint = b->GetLocalPoint(anchor);
}

void JointConstraint::Solve() {
    
}
