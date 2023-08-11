#ifndef FORCE_H
#define FORCE_H

#pragma once

#include "./Vec2.h"
#include "./Particle.h"

struct Force {
    static Vec2 GenerateDragForce(Particle& particle, float dragCoefficient);
    static Vec2 GenerateFrictionForce(Particle& particle, float frictionCoefficient);
};


#endif //FORCE_H
