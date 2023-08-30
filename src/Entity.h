#ifndef ENTITIES_H
#define ENTITIES_H

#pragma once

#include "./Physics/Body.h"
#include "./Physics/Contact.h"

// Forward declaration
struct SDL_Texture;

struct Entity
{
    Entity() = default;
    Entity(Body* body, const char* fileName);
    Entity(const Shape& shape, float x, float y, float mass);
    Entity(Body* body);
    ~Entity();
    
    Body* body = nullptr;
    
    SDL_Texture* texture = nullptr;
    
public:
    void SetTexture(const char* fileName);
    void Draw(bool debugMode = false) const;
};


#endif
