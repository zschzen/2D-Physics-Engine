#ifndef BODY_H
#define BODY_H

#pragma once
#include "Vec2.h"
#include "SDL_stdinc.h"

struct Body {
    Body() = default;
    Body(float x, float y, float mass);
    ~Body();

    Vec2 position{};
    Vec2 velocity{};
    Vec2 acceleration{};

    float mass{};
    float inverseMass{};
    int radius{};

    Uint32 color = 0xFFFFFFFF;

public:
    void AddForce(const Vec2& force);
    void Integrate(float deltaTime);

private:
    Vec2 netForce{};

    void ClearForces();
};

#endif