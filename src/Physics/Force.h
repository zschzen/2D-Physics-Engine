#ifndef FORCE_H
#define FORCE_H

#pragma once

#include "Math/Vec2.h"
#include "./Body.h"

struct Force {
    static Vec2 GenerateDragForce(const Body& particle, float dragCoefficient);
    static Vec2 GenerateFrictionForce(const Body& particle, float frictionCoefficient);
    static Vec2 GenerateGravitationalForce(const Body& a, const Body& b, float G, float minDistance, float maxDistance);
    static Vec2 GenerateSpringForce(const Body& a, Vec2 anchor, float restLength, float springConstant);
    static Vec2 GenerateSpringForce(const Body& a, const Body& b, float restLength, float springConstant);
};


#endif //FORCE_H
