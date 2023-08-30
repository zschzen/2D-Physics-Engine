#include "Entity.h"

#include <SDL_image.h>

#include "./Physics/Body.h"
#include "./Graphics.h"

Entity::Entity(Body* body, const char* fileName)
{
    this->body = body;
    SetTexture(fileName);
}

Entity::Entity(const Shape& shape, float x, float y, float mass)
{
    body = new Body(shape, x, y, mass);
}

Entity::Entity(Body* body)
{
    this->body = body;
}

Entity::~Entity()
{
    delete body;
    body = nullptr;
    SDL_DestroyTexture(texture);
    texture = nullptr;
}

void Entity::SetTexture(const char* fileName)
{
    SDL_Surface* surface = IMG_Load(fileName);
    if (!surface) {
        printf("Failed to load texture: %s\n", fileName);
        return;
    }
    
    texture = SDL_CreateTextureFromSurface(Graphics::renderer, surface);
    SDL_FreeSurface(surface);
}

void Entity::Draw(bool debugMode) const
{
    Uint32 color = body->isColliding ? 0xFF0000FF : 0xFFFFFFFF;

    switch (body->shape->GetType()) {
        case ShapeType::CIRCLE: {
            CircleShape* circle = dynamic_cast<CircleShape*>(body->shape);
            if (debugMode || !texture)
                Graphics::DrawCircle(body->position.x, body->position.y, circle->radius, body->rotation, color);
            else
                Graphics::DrawTexture(body->position.x, body->position.y, circle->radius * 2, circle->radius * 2, body->rotation, texture);
            break;
        }
        case ShapeType::BOX: {
            BoxShape* box = dynamic_cast<BoxShape*>(body->shape);
            if (debugMode || !texture)
                Graphics::DrawPolygon(body->position.x, body->position.y, box->worldVertices, color);
            else
                Graphics::DrawTexture(body->position.x, body->position.y, box->width, box->height, body->rotation, texture);
            break;
        }
        case ShapeType::POLYGON: {
            PolygonShape* polygon = dynamic_cast<PolygonShape*>(body->shape);
            Graphics::DrawPolygon(body->position.x, body->position.y, polygon->worldVertices, color);
            break;
        }
        default:
            break;
    }
}
