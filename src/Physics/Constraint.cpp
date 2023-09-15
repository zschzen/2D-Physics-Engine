#include "Constraint.h"

#include <algorithm>

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
    result.rows[2][2] = a->inverseI;
    // b
    result.rows[3][3] = b->inverseMass;
    result.rows[4][4] = b->inverseMass;
    result.rows[5][5] = b->inverseI;
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
JointConstraint::JointConstraint() : Constraint() {
    cachedLambda.Zero();
}

JointConstraint::JointConstraint(Body *a, Body *b, const Vec2 &anchor) : Constraint() {
    this->a = a;
    this->b = b;
    this->aPoint = a->GetLocalPoint(anchor);
    this->bPoint = b->GetLocalPoint(anchor);
    cachedLambda.Zero();
}

void JointConstraint::PreSolve(float deltaTime) {
    // Get the anchor point position in world space
    const Vec2 pa = a->GetWorldPoint(aPoint);
    const Vec2 pb = b->GetWorldPoint(bPoint);

    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;

    jacobian.Zero();

    Vec2 J1 = (pa - pb) * 2.0;
    jacobian.rows[0][0] = J1.x; // A linear velocity.x
    jacobian.rows[0][1] = J1.y; // A linear velocity.y

    float J2 = ra.Cross(pa - pb) * 2.0;
    jacobian.rows[0][2] = J2;   // A angular velocity

    Vec2 J3 = (pb - pa) * 2.0;
    jacobian.rows[0][3] = J3.x; // B linear velocity.x
    jacobian.rows[0][4] = J3.y; // B linear velocity.y

    float J4 = rb.Cross(pb - pa) * 2.0;
    jacobian.rows[0][5] = J4;   // B angular velocity

    // Compute the final impulses with direction and magnitude
    const MatMN Jt = jacobian.Transpose();
    VecN impulses = Jt * cachedLambda;

    // Apply the impulses to both A and B
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse
    
    // Compute the bias (baumgarte stabilization)
    static float beta = 0.1f;
    const float C = (pb - pa).Dot(pb - pa);                 // Positional error
    bias = (beta / deltaTime) * C;
}

void JointConstraint::Solve() {
    const VecN V = GetVelocities();
    const MatMN invM = GetInvMassMatrix();

    const MatMN J = jacobian;
    const MatMN Jt = jacobian.Transpose();

    // Calculate the numerator
    MatMN lhs = J * invM * Jt; // A
    VecN rhs = J * V * -1.0f;  // b
    rhs[0] -= bias;

    // Solve the values of lambda using Ax=b (Gaus-Seidel method)
    VecN lambda = MatMN::SolveGaussSeidel(lhs, rhs);
    cachedLambda += lambda;

    // Compute the final impulses with direction and magnitude
    VecN impulses = Jt * lambda;

    // Apply the impulses to both A and B
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse
}

void JointConstraint::PostSolve() {}

///////////////////////////////////////////////////////////////////////////////
// PenerationConstraint
///////////////////////////////////////////////////////////////////////////////
PenetrationConstraint::PenetrationConstraint() : Constraint() {
    cachedLambda.Zero();
}

PenetrationConstraint::PenetrationConstraint(Body* a, Body* b, const Vec2& aCollisionPoint, const Vec2& bCollisionPoint, const Vec2& normal) : Constraint() {
    this->a = a;
    this->b = b;
    this->aPoint = a->GetLocalPoint(aCollisionPoint);
    this->bPoint = b->GetLocalPoint(bCollisionPoint);
    this->normal = a->GetLocalPoint(normal);
    cachedLambda.Zero();
}

void PenetrationConstraint::PreSolve(float deltaTime) {
    // Get the anchor point position in world space
    const Vec2 pa = a->GetWorldPoint(aPoint);
    const Vec2 pb = b->GetWorldPoint(bPoint);
    Vec2 n = a->GetWorldPoint(normal);

    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;

    jacobian.Zero();

    Vec2 J1 = -n;
    jacobian.rows[0][0] = J1.x; // A linear velocity.x
    jacobian.rows[0][1] = J1.y; // A linear velocity.y

    float J2 = -ra.Cross(n);
    jacobian.rows[0][2] = J2;   // A angular velocity

    Vec2 J3 = n;
    jacobian.rows[0][3] = J3.x; // B linear velocity.x
    jacobian.rows[0][4] = J3.y; // B linear velocity.y

    float J4 = rb.Cross(n);
    jacobian.rows[0][5] = J4;   // B angular velocity
    
    // Second row of the jacobian (tangent - friction)
    friction = std::max(a->friction, b->friction);
    if (friction > 0.0f) {
        Vec2 t = n.Normal();
        jacobian.rows[1][0] = -t.x; // A linear velocity.x
        jacobian.rows[1][1] = -t.y; // A linear velocity.y
        jacobian.rows[1][2] = -ra.Cross(t); // A angular velocity
        jacobian.rows[1][3] = t.x; // B linear velocity.x
        jacobian.rows[1][4] = t.y; // B linear velocity.y
        jacobian.rows[1][5] = rb.Cross(t); // B angular velocity
    }
    
    // Compute the final impulses with direction and magnitude
    const MatMN Jt = jacobian.Transpose();
    VecN impulses = Jt * cachedLambda;

    // Apply the impulses to both A and B
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse

    // Compute the bias (baumgarte stabilization)
    static float beta = 0.1f;
    float C = (pb - pa).Dot(-n);                           // Positional error
    C = std::min(0.0f, C + 0.01f);                         // Clamp the error
    
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    float vrelDotNormal = (va - vb).Dot(n);                // Relative velocity
    
    float e = std::min(a->restitution, b->restitution);    // Restitution
    
    // Bias term relative to restitution
    bias = (beta / deltaTime) * C + (e * vrelDotNormal);
}

void PenetrationConstraint::Solve() {
    const VecN V = GetVelocities();
    const MatMN invM = GetInvMassMatrix();

    const MatMN J = jacobian;
    const MatMN Jt = jacobian.Transpose();

    // Calculate the numerator
    MatMN lhs = J * invM * Jt; // A
    VecN rhs = J * V * -1.0f;  // b
    rhs[0] -= bias;

    // Solve the values of lambda using Ax=b (Gaus-Seidel method)
    VecN lambda = MatMN::SolveGaussSeidel(lhs, rhs);
    
    // Accumulate the lambda values
    VecN oldLambda = cachedLambda;
    cachedLambda += lambda;
    cachedLambda[0] = std::max(0.0f, cachedLambda[0]);

    // Keep friction values between  -(λn*µ) and (λn*µ)
    if (friction > 0.0f) {
        const float maxFriction = friction * cachedLambda[0];
        cachedLambda[1] = std::clamp(cachedLambda[1], -maxFriction, maxFriction);
    }
    
    lambda = cachedLambda - oldLambda;

    // Compute the final impulses with direction and magnitude
    VecN impulses = Jt * lambda;

    // Apply the impulses to both A and B
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse
}

void PenetrationConstraint::PostSolve() {}