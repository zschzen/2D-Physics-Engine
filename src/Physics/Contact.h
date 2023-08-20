#ifndef CONTACT_H
#define CONTACT_H

#pragma once

#include "Vec2.h"

// Forward declaration
struct Body;

struct Contact
{
    Contact() = default;
    ~Contact() = default;

    Body* a = nullptr;
    Body* b = nullptr;

    Vec2 start{};
    Vec2 end{};

    Vec2 normal{};
    float depth{};

    void ResolvePenetration() const;
};

#endif
