#include "Body.h"

Body::Body(const Shape& shape, float x, float y, float mass)
{
    this->shape = shape.Clone();
    this->position = Vec2(x, y);

    this->mass = mass;
    this->inverseMass = (mass != 0.0f) ? 1.0f / mass : 0.0f;
    
    this->I = shape.GetMomentOfInertia() * mass;
    this->inverseI = (I != 0.0f) ? 1.0f / I : 0.0f;
}

Body::~Body()
{
    delete shape;
}

void Body::AddForce(const Vec2& force)
{
    netForce += force;
}

void Body::AddTorque(float torque)
{
    netTorque += torque;
}

void Body::IntegrateLinear(float deltaTime)
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

void Body::IntegrateAngular(float deltaTime)
{
    // Find angular acceleration based on torque
    // Fr = I * a
    angularAcceleration = netTorque * inverseI;

    // Integrate the acceleration to find the velocity
    angularVelocity += angularAcceleration * deltaTime;

    // Integrate the velocity to find the rotation angle
    rotation += angularVelocity * deltaTime;

    // Clear the net force
    ClearTorque();
}

void Body::Update(float dt)
{
    IntegrateLinear(dt);
    IntegrateAngular(dt);

    const bool isPolygon = shape->GetType() == ShapeType::POLYGON || shape->GetType() == ShapeType::BOX;
    if (!isPolygon) return;

    PolygonShape* polygonShape = dynamic_cast<PolygonShape*>(shape);
    polygonShape->UpdateVertices(rotation, position);
}

void Body::ClearForces()
{
    netForce = Vec2(0, 0);
}

void Body::ClearTorque()
{
    netTorque = 0.0f;
}