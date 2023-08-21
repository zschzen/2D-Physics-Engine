#include "Contact.h"

#include "Body.h"

void Contact::ResolvePenetration() const
{
    if (a->IsStatic() && b->IsStatic()) return;
    
    const float totalInverseMass = a->inverseMass + b->inverseMass;
    const float da = depth / totalInverseMass * a->inverseMass;
    const float db = depth / totalInverseMass * b->inverseMass;

    a->position -= normal * da;
    b->position += normal * db;
}

void Contact::ResolveCollision() const
{
    // Apply position correction using project method
    ResolvePenetration();

    // Define elasticity as minimum restitution
    const float e = std::min(a->restitution, b->restitution);

    // Calculate relative velocity
    const Vec2 rv = a->velocity - b->velocity;
    
    // Calculate the relative velocity in terms of the normal direction
    const float velAlongNormal = rv.Dot(normal);

    // Calculate the collision impulse (J)
    const Vec2 impulseDirection = normal;
    const float impulseMagnitude = -(1.0f + e) * velAlongNormal / (a->inverseMass + b->inverseMass);
    Vec2 Jn = impulseDirection * impulseMagnitude;

    // Apply the impulse
    a->ApplyImpulse(Jn);
    b->ApplyImpulse(-Jn);
}
