#include "CollisionDetection.h"

#include "Body.h"

bool CollisionDetection::IsColliding(const Body* a, const Body* b)
{
    ShapeType typeA = a->shape->GetType();
    ShapeType typeB = b->shape->GetType();
    
    if (typeA == ShapeType::CIRCLE && typeB == ShapeType::CIRCLE)
    {
        return IsCollidingCircleCircle(a, b);
    }

    return false;
}

bool CollisionDetection::IsCollidingCircleCircle(const Body* a, const Body* b)
{
    // Get the distance between the two circles
    float distanceSquared = (a->position - b->position).MagnitudeSquared();

    // Get the sum of the radius
    float radiusA = dynamic_cast<CircleShape*>(a->shape)->radius;
    float radiusB = dynamic_cast<CircleShape*>(b->shape)->radius;
    float radiusSum = radiusA + radiusB;
    
    // Check if the distance is less than the sum of the radii
    return distanceSquared <= radiusSum * radiusSum;
}