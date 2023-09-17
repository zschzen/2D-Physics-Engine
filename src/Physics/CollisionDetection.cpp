#include "CollisionDetection.h"

#include <limits>

#include "Body.h"
#include "Contact.h"

bool CollisionDetection::IsColliding(Body* a, Body* b, std::vector<Contact>& contacts) {
    ShapeType typeA = a->shape->GetType();
    ShapeType typeB = b->shape->GetType();

    if (typeA == ShapeType::CIRCLE && typeB == ShapeType::CIRCLE)
        return IsCollidingCircleCircle(a, b, contacts);

    if ((typeA == ShapeType::POLYGON || typeA == ShapeType::BOX) &&
        (typeB == ShapeType::POLYGON || typeB == ShapeType::BOX))
        return IsCollidingPolygonPolygon(a, b, contacts);

    if ((typeA == ShapeType::POLYGON || typeA == ShapeType::BOX) &&
        typeB == ShapeType::CIRCLE)
        return IsCollidingPolygonCircle(a, b, contacts);

    if ((typeB == ShapeType::POLYGON || typeB == ShapeType::BOX) &&
        typeA == ShapeType::CIRCLE)
        return IsCollidingPolygonCircle(b, a, contacts);

    return false;
}


bool CollisionDetection::IsCollidingCircleCircle(Body *a, Body *b, std::vector<Contact> &contacts) {
    // Get the circle shapes
    CircleShape *circleA = (CircleShape *) a->shape;
    CircleShape *circleB = (CircleShape *) b->shape;

    // Get the distance and the sum of the radius
    const Vec2 distance = b->position - a->position;
    const float radiusSum = circleA->radius + circleB->radius;

    // Check if its colliding
    bool isColliding = distance.MagnitudeSquared() <= (radiusSum * radiusSum);
    if (!isColliding) return false;

    // Set the contact
    Contact contact{};
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
    
    contacts.push_back(contact);
    return true;
}

bool CollisionDetection::IsCollidingPolygonPolygon(Body *a, Body *b, std::vector<Contact> &contacts) {
    const PolygonShape *aPolygonShape = (PolygonShape *) a->shape;
    const PolygonShape *bPolygonShape = (PolygonShape *) b->shape;

    Vec2 aIndexReferenceEdge, aSupportPoint;
    float abSeparation = aPolygonShape->FindMinSeparation(*bPolygonShape, aIndexReferenceEdge, aSupportPoint);
    if (abSeparation >= 0) return false;

    Vec2 bIndexReferenceEdge, bSupportPoint;
    float baSeparation = bPolygonShape->FindMinSeparation(*aPolygonShape, bIndexReferenceEdge, bSupportPoint);
    if (baSeparation >= 0) return false;

    Contact contact{};
    contact.a = a;
    contact.b = b;

    if (abSeparation > baSeparation) {
        contact.depth = -abSeparation;
        contact.normal = aIndexReferenceEdge.Normal();
        contact.start = aSupportPoint;
        contact.end = aSupportPoint + contact.normal * contact.depth;
    } else {
        contact.depth = -baSeparation;
        contact.normal = -bIndexReferenceEdge.Normal();
        contact.start = bSupportPoint - contact.normal * contact.depth;
        contact.end = bSupportPoint;
    }
    
    contacts.push_back(contact);
    return true;
}

bool CollisionDetection::IsCollidingPolygonCircle(Body *polygon, Body *circle, std::vector<Contact> &contacts) {
    const PolygonShape *polygonShape = dynamic_cast<PolygonShape *>(polygon->shape);
    const CircleShape *circleShape = dynamic_cast<CircleShape *>(circle->shape);
    const std::vector<Vec2> &polygonVertices = polygonShape->worldVertices;

    bool isOutside = false;
    Vec2 minCurrVertex;
    Vec2 minNextVertex;
    float distanceCircleEdge = std::numeric_limits<float>::lowest();

    // Loop all the edges of the polygon/box finding the nearest edge to the circle center
    for (int i = 0; i < polygonVertices.size(); i++) {
        int currVertex = i;
        int nextVertex = (i + 1) % polygonVertices.size();
        Vec2 edge = polygonShape->GetEdge(currVertex);
        Vec2 normal = edge.Normal();

        // Compare the circle center with the rectangle vertex
        Vec2 vertexToCircleCenter = circle->position - polygonVertices[currVertex];
        float projection = vertexToCircleCenter.Dot(normal);

        // If we found a dot product projection that is in the positive/outside side of the normal
        if (projection > 0) {
            // Circle center is outside the polygon
            distanceCircleEdge = projection;
            minCurrVertex = polygonVertices[currVertex];
            minNextVertex = polygonVertices[nextVertex];
            isOutside = true;
            break;
        } else {
            // Circle center is inside the rectangle, find the min edge (the one with the least negative projection)
            if (projection > distanceCircleEdge) {
                distanceCircleEdge = projection;
                minCurrVertex = polygonVertices[currVertex];
                minNextVertex = polygonVertices[nextVertex];
            }
        }
    }

    Contact contact{};
    
    // If inside the polygon, it's a collision!
    if (!isOutside) {
        contact.a = polygon;
        contact.b = circle;
        contact.depth = circleShape->radius - distanceCircleEdge;
        contact.normal = (minNextVertex - minCurrVertex).Normal();
        contact.start = circle->position - (contact.normal * circleShape->radius);
        contact.end = contact.start + (contact.normal * contact.depth);

        return true;
    }

    // Check region A
    Vec2 v1 = circle->position - minCurrVertex; // vector from the nearest vertex to the circle center
    Vec2 v2 = minNextVertex - minCurrVertex; // the nearest edge (from curr vertex to next vertex)
    if (v1.Dot(v2) < 0) {
        // Distance from vertex to circle center is greater than radius... no collision
        if (v1.Magnitude() > circleShape->radius) return false;

        // Detected collision
        contact.a = polygon;
        contact.b = circle;
        contact.depth = circleShape->radius - v1.Magnitude();
        contact.normal = v1.Normalize();
        contact.start = circle->position + (contact.normal * -circleShape->radius);
        contact.end = contact.start + (contact.normal * contact.depth);
    } else {
        // Check region B
        v1 = circle->position - minNextVertex; // vector from the next nearest vertex to the circle center
        v2 = minCurrVertex - minNextVertex;   // the nearest edge
        if (v1.Dot(v2) < 0) {
            // Distance from vertex to circle center is greater than radius... no collision
            if (v1.Magnitude() > circleShape->radius) return false;

            // Detected collision in region B:
            contact.a = polygon;
            contact.b = circle;
            contact.depth = circleShape->radius - v1.Magnitude();
            contact.normal = v1.Normalize();
            contact.start = circle->position + (contact.normal * -circleShape->radius);
            contact.end = contact.start + (contact.normal * contact.depth);
        } else {
            // Region C
            // Distance between the closest distance and the circle center is greater than the radius.
            if (distanceCircleEdge > circleShape->radius) return false;

            // Detected collision in region C:
            contact.a = polygon;
            contact.b = circle;
            contact.depth = circleShape->radius - distanceCircleEdge;
            contact.normal = (minNextVertex - minCurrVertex).Normal();
            contact.start = circle->position - (contact.normal * circleShape->radius);
            contact.end = contact.start + (contact.normal * contact.depth);
        }
    }
    contacts.push_back(contact);
    return true;
}
