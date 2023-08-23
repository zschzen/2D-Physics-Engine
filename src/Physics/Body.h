#ifndef BODY_H
#define BODY_H

#pragma once
#include "Vec2.h"
#include "Shape.h"
#include "SDL_stdinc.h"

struct Body {
    Body() = default;
    Body(const Shape& shape, float x, float y, float mass);
    ~Body();

    // Linear motion
    Vec2 position{};
    Vec2 velocity{};
    Vec2 acceleration{};
    
    // Angular motion
    float rotation{};
    float angularVelocity{};
    float angularAcceleration{};

    // Mass and moment of inertia
    float mass{};
    float inverseMass{};
    float I{};
    float inverseI{};

    // Forces and torques
    Vec2 netForce{};
    float netTorque{};

    // Collision data
    bool isColliding{};

    // Restitution ([e]lasticity)
    float restitution = 1.0f;
    
    // Friction
    float friction = 1.0f;
    
public:
    Shape* shape = nullptr;

    Uint32 color = 0xFFFFFFFF;

public:
    bool IsStatic() const;

    void AddForce(const Vec2& force);
    void AddTorque(float torque);
    void ApplyImpulse(const Vec2& j);
    void ApplyImpulse(const Vec2& j, const Vec2& contactVector);
    
    void IntegrateLinear(float deltaTime);
    void IntegrateAngular(float deltaTime);

    void Update(float deltaTime);

private:
    void ClearForces();
    void ClearTorque();
};

#endif