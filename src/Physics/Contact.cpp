#include "Contact.h"

#include "Body.h"

void Contact::ResolvePenetration() const {
    if (a->IsStatic() && b->IsStatic()) return;

    const float totalInverseMass = a->inverseMass + b->inverseMass;
    const float da = depth / totalInverseMass * a->inverseMass;
    const float db = depth / totalInverseMass * b->inverseMass;

    a->position -= normal * da;
    b->position += normal * db;
}

void Contact::ResolveCollision() const {
    // Apply position correction using project method
    ResolvePenetration();

    // Define elasticity as minimum restitution
    const float e = std::min(a->restitution, b->restitution);

    // Calculate relative velocity
    Vec2 ra = end - a->position;
    Vec2 rb = start - b->position;
    // 2D Cross product between a vector and a scalar
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    const Vec2 vrel = va - vb;

    // Calculate the relative velocity in terms of the normal direction
    const float velAlongNormal = vrel.Dot(normal);

    // Calculate the collision impulse (J)
    const Vec2 impulseDirection = normal;
    const float impulseMagnitude = -(1.0f + e) * velAlongNormal / ((a->inverseMass + b->inverseMass)
                                                                   + ra.Cross(normal) * ra.Cross(normal) * a->inverseI
                                                                   + rb.Cross(normal) * rb.Cross(normal) * b->inverseI);
    Vec2 Jn = impulseDirection * impulseMagnitude;

    // Apply the impulse
    a->ApplyImpulse(Jn, ra);
    b->ApplyImpulse(-Jn, rb);
}
