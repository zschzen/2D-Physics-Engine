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
    const float f = std::min(a->friction, b->friction);

    // Calculate relative velocity
    Vec2 ra = end - a->position;
    Vec2 rb = start - b->position;
    // 2D Cross product between a vector and a scalar
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    const Vec2 vrel = va - vb;

    // Calculate the relative velocity in terms of the normal direction
    const float velAlongNormal = vrel.Dot(normal);

    // Calculate the collision impulse (J) along the normal
    const Vec2 impulseDirectionN = normal;
    const float impulseMagnitudeN = -(1.0f + e) * velAlongNormal / ((a->inverseMass + b->inverseMass)
                                                                    + ra.Cross(normal) * ra.Cross(normal) * a->inverseI
                                                                    + rb.Cross(normal) * rb.Cross(normal) * b->inverseI);
    Vec2 Jn = impulseDirectionN * impulseMagnitudeN;

    // Calculate the collision impulse along the tangent
    const Vec2 tangent = normal.Normal();
    float vrelDotTangent = vrel.Dot(tangent);
    const Vec2 frictionImpulseDirection = tangent;
    const float frictionImpulseMagnitude = f * -(1.0f + e) * vrelDotTangent / ((a->inverseMass + b->inverseMass)
                                                                               + ra.Cross(tangent) * ra.Cross(tangent) * a->inverseI
                                                                               + rb.Cross(tangent) * rb.Cross(tangent) * b->inverseI);
    Vec2 Jt = frictionImpulseDirection * frictionImpulseMagnitude;
    
    Vec2 J = Jn + Jt;
    
    // Apply the impulse
    a->ApplyImpulse(J, ra);
    b->ApplyImpulse(-J, rb);
}
