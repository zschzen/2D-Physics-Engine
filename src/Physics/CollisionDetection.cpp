#include "CollisionDetection.h"

#include "Body.h"

#include "Contact.h"

bool CollisionDetection::IsColliding(Body* a, Body* b, Contact& contact)
{
    ShapeType typeA = a->shape->GetType();
    ShapeType typeB = b->shape->GetType();
    
    if (typeA == ShapeType::CIRCLE && typeB == ShapeType::CIRCLE)
    {
        return IsCollidingCircleCircle(a, b, contact);
    }

    return false;
}

bool CollisionDetection::IsCollidingCircleCircle(Body* a, Body* b, Contact& contact)
{
    // Get the circle shapes
    CircleShape* circleA = (CircleShape*) a->shape;
    CircleShape* circleB = (CircleShape*) b->shape;

    // Get the distance and the sum of the radius
    const Vec2 distance = b->position - a->position;
    const float radiusSum = circleA->radius + circleB->radius;

    // Check if its colliding
    bool isColliding = distance.MagnitudeSquared() <= (radiusSum * radiusSum);
    if (!isColliding) return false;

    // Set the contact
    contact.a = a;
    contact.b = b;

    // Set the contact normal
    contact.normal = distance;
    contact.normal.Normalize();

    // Set the contact points
    contact.start = b->position - contact.normal * circleB->radius;
    contact.end = a->position + contact.normal * circleA->radius;

    // Set the contact depth
    contact.depth = (contact.end - contact.start).Magnitude();

    return true;
}
