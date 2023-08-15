#include "Body.h"

Body::Body(float x, float y, float mass)
{
    this->position = Vec2(x, y);
    this->mass = mass;

    this->inverseMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
}

Body::~Body()
{

}

void Body::AddForce(const Vec2& force)
{
    netForce += force;
}

void Body::Integrate(float deltaTime)
{
    // Fr = m * a
    acceleration = netForce * inverseMass;

    // Integrate the acceleration to find the velocity
    velocity += acceleration * deltaTime;

    // Integrate the velocity to find the position
    position += velocity * deltaTime;

    // Clear the net force
    ClearForces();
}

void Body::ClearForces()
{
    netForce = Vec2(0, 0);
}