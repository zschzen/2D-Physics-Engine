#ifndef BODY_H
#define BODY_H

#pragma once

#include "Math/Vec2.h"
#include "Shape.h"

// Forward declaration
struct SDL_Texture;

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

    // Restitution ([e]lasticity)
    float restitution = 1.0f;
    
    // Friction
    float friction = 1.0f;
	
	// Gravity scale
	float gravityScale = 1.0f;
    
public:
    Shape* shape = nullptr;

public:
    bool IsStatic() const;

    void AddForce(const Vec2& force);
    void AddTorque(float torque);
    void ApplyImpulseLinear(const Vec2& j);
    void ApplyImpulseAngular(const float j);
    void ApplyImpulseAtPoint(const Vec2& j, const Vec2& contactVector);
    
    void IntegrateForces(const float deltaTime);
    void IntegrateVelocities(const float deltaTime);

    Vec2 GetLocalPoint(const Vec2 &point) const;
    Vec2 GetWorldPoint(const Vec2 &vec2) const;
	
public:
	void SetTexture(const char* fileName);

private:
    void ClearForces();
    void ClearTorque();
	
public:
	SDL_Texture* texture = nullptr;
};

#endif