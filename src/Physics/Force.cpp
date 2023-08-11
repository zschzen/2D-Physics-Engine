#include "Force.h"

#include "./Constants.h"

Vec2 Force::GenerateDragForce(Particle& particle, float dragCoefficient) {
    Vec2 dragForce = Vec2();

    if (particle.velocity.MagnitudeSquared() > 0.0f) {
        // Drag direction is the opposite of the velocity
        Vec2 dragDirection = -particle.velocity.UnitVector();

        // Calculate the magnitude of the drag force
        float dragMagnitude = dragCoefficient * particle.velocity.MagnitudeSquared();

        // Generate the final drag force
        dragForce = dragDirection * dragMagnitude;
    }

    return dragForce;
}

Vec2 Force::GenerateFrictionForce(Particle& particle, float frictionCoefficient) {
    Vec2 frictionForce = Vec2(0, 0);

    // Calculate the friction direction (inverse of velocity unit vector)
    Vec2 frictionDirection = particle.velocity.UnitVector() * -1.0;

    // Calculate the friction magnitude
    float frictionMagnitude = frictionCoefficient;

    // Calculate the final friction force
    frictionForce = frictionDirection * frictionMagnitude;

    return frictionForce;
}
