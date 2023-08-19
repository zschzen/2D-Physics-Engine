#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#pragma once

// Forward declaration
struct Body;

struct CollisionDetection
{
    static bool IsColliding(const Body* a, const Body* b);
    static bool IsCollidingCircleCircle(const Body* a, const Body* b);
};


#endif
