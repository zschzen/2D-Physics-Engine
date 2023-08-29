#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#pragma once

// Forward declaration
struct Contact;
struct Body;
struct PolygonShape;

struct CollisionDetection
{
    static bool IsColliding(Body* a, Body* b, Contact& contact);
    static bool IsCollidingCircleCircle(Body* a, Body* b, Contact& contact);
    static bool IsCollidingPolygonPolygon(Body* a, Body* b, Contact& contact);
    static bool IsCollidingPolygonCircle(Body* polygon, Body* circle, Contact& contact);
};


#endif
