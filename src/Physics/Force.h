#ifndef FORCE_H
#define FORCE_H

#pragma once

#include "./Vec2.h"
#include "./Particle.h"

struct Force {
    static Vec2 GenerateDragForce(const Particle& particle, float dragCoefficient);
    static Vec2 GenerateFrictionForce(const Particle& particle, float frictionCoefficient);
    static Vec2 GenerateGravitationalForce(const Particle& a, const Particle& b, float G, float minDistance, float maxDistance);
    static Vec2 GenerateSpringForce(const Particle& a, Vec2 anchor, float restLength, float springConstant);
    static Vec2 GenerateSpringForce(const Particle& a, const Particle& b, float restLength, float springConstant);
};


#endif //FORCE_H
