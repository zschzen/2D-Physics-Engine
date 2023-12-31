#include "Body.h"

#include <cmath>
#include <limits>

#include <SDL_image.h>

#include "../Graphics.h"

Body::Body(const Shape& shape, float x, float y, float mass)
{
    this->shape = shape.Clone();
    this->position = Vec2(x, y);

    this->mass = mass;
    this->inverseMass = (mass != 0.0f) ? 1.0f / mass : 0.0f;
    
    this->I = shape.GetMomentOfInertia() * mass;
    this->inverseI = (I != 0.0f) ? 1.0f / I : 0.0f;

    this->shape->UpdateVertices(rotation, position);
}

Body::~Body()
{
    delete shape;
    shape = nullptr;
}

bool Body::IsStatic() const
{
    return std::abs(inverseMass) < std::numeric_limits<float>::epsilon();
}

void Body::AddForce(const Vec2& force)
{
    netForce += force;
}

void Body::AddTorque(float torque)
{
    netTorque += torque;
}

////////////////////////////////////////////////////////
// Impulse is an instantaneous change in velocity,
// inversely proportional to the mass of the object.
// Momentum: P = m * v
// Impulse J is the change in momentum: J = ΔP = m * Δv
// The change in velocity is Δv = J / m
////////////////////////////////////////////////////////
void Body::ApplyImpulseLinear(const Vec2 &j)
{
    if (IsStatic()) return;

    velocity += j * inverseMass;
}

void Body::ApplyImpulseAngular(const float j)
{
    if (IsStatic()) return;

    angularVelocity += j * inverseI;
}

void Body::ApplyImpulseAtPoint(const Vec2 &j, const Vec2 &contactVector)
{
    if (IsStatic()) return;

    velocity += j * inverseMass;
    angularVelocity += inverseI * contactVector.Cross(j);
}

void Body::IntegrateForces(const float deltaTime)
{
    if (IsStatic()) return;
    
    // Find linear acceleration based on net force (Fr = m * a)
    acceleration = netForce * inverseMass;

    // Integrate the acceleration to find the velocity
    velocity += acceleration * deltaTime;

    // Find angular acceleration based on torque
    // Fr = I * a
    angularAcceleration = netTorque * inverseI;

    // Integrate the acceleration to find the velocity
    angularVelocity += angularAcceleration * deltaTime;

    // Clear the net force and torque
    ClearForces();
    ClearTorque();
}

/*
 * Integrate the velocity to find the position
 * Called before Constraint::Solve()
 */
void Body::IntegrateVelocities(const float deltaTime)
{
    if (IsStatic()) return;

    // Integrate the velocity to find the position
    position += velocity * deltaTime;

    // Integrate the velocity to find the rotation angle
    rotation += angularVelocity * deltaTime;
    
    // Update the vertices of the shape
    shape->UpdateVertices(rotation, position);
}

void Body::ClearForces()
{
    netForce = Vec2(0, 0);
}

void Body::ClearTorque()
{
    netTorque = 0.0f;
}

void Body::SetTexture(const char* fileName)
{
	SDL_Surface* surface = IMG_Load(fileName);
	if (!surface) {
		printf("Failed to load texture: %s\n", fileName);
		return;
	}

	texture = SDL_CreateTextureFromSurface(Graphics::renderer, surface);
	SDL_FreeSurface(surface);
}

Vec2 Body::GetLocalPoint(const Vec2 &point) const
{
    // inverse translation
    float transformedX = point.x - position.x;
    float transformedY = point.y - position.y;
    // inverse rotation matrix
    float rotatedX = transformedX * std::cos(-rotation) - transformedY * std::sin(-rotation);
    float rotatedY = transformedY * std::cos(-rotation) + transformedX * std::sin(-rotation);
    return {rotatedX, rotatedY};
}

Vec2 Body::GetWorldPoint(const Vec2 &vec2) const
{
    Vec2 rotated = vec2.Rotate(rotation);
    return rotated + position;
}