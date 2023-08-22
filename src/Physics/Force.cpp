#include "Force.h"

#include "./Constants.h"
#include <algorithm>

Vec2 Force::GenerateDragForce(const Body& particle, float dragCoefficient) {
    Vec2 dragForce = Vec2();

    const float velocitySquared = particle.velocity.MagnitudeSquared();
    
    if (velocitySquared > 0.0f) {
        // Drag direction is the opposite of the velocity
        Vec2 dragDirection = -particle.velocity.UnitVector();

        // Calculate the magnitude of the drag force
        float dragMagnitude = dragCoefficient * velocitySquared;

        // Generate the final drag force
        dragForce = dragDirection * dragMagnitude;
    }

    return dragForce;
}

Vec2 Force::GenerateFrictionForce(const Body& particle, float frictionCoefficient) {
    Vec2 frictionForce = Vec2(0, 0);

    // Calculate the friction direction (inverse of velocity unit vector)
    Vec2 frictionDirection = -particle.velocity.UnitVector();

    // Calculate the friction magnitude
    float frictionMagnitude = frictionCoefficient;

    // Calculate the final friction force
    frictionForce = frictionDirection * frictionMagnitude;

    return frictionForce;
}

Vec2 Force::GenerateGravitationalForce(const Body& a, const Body& b, float G, float minDistance, float maxDistance) {
    // Calculate the distance between the two objects
    Vec2 d = (b.position - a.position);

    float distanceSquared = d.MagnitudeSquared();

    // Clamp the values of the distance (to allow for some interesting visual effects)
    distanceSquared = std::clamp(distanceSquared, minDistance, maxDistance);

    // Calculate the direction of the attraction force
    Vec2 attractionDirection = d.UnitVector();

    // Calculate the strength of the attraction force
    float attractionMagnitude = G * (a.mass * b.mass) / distanceSquared;

    // Calculate the final resulting attraction force vector
    Vec2 attractionForce = attractionDirection * attractionMagnitude;
    return attractionForce;
}

Vec2 Force::GenerateSpringForce(const Body &a, Vec2 anchor, float restLength, float springConstant) {
    // Calculate the distance between the particle and the anchor
    Vec2 d = (a.position - anchor);

    // Find spring displacement
    float displacement = d.Magnitude() - restLength;

    // Calculate the direction and magnitude of the spring force
    Vec2 springDirection = d.UnitVector();
    float springMagnitude = -springConstant * displacement;

    Vec2 springForce = springDirection * springMagnitude;
    return springForce;
}

Vec2 Force::GenerateSpringForce(const Body& a, const Body& b, float restLength, float springConstant) {
    return GenerateSpringForce(a, b.position, restLength, springConstant);
}
