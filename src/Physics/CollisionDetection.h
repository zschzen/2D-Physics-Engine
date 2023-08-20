#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#pragma once

// Forward declaration
struct Contact;
struct Body;

struct CollisionDetection
{
    static bool IsColliding(Body* a, Body* b, Contact& contact);
    static bool IsCollidingCircleCircle(Body* a, Body* b, Contact& contact);
};


#endif
