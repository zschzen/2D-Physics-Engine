#include "Shape.h"

CircleShape::CircleShape(float radius) : radius(radius) {}

PolygonShape::PolygonShape(const std::vector<Vec2> &vertices) : vertices(vertices) {}

BoxShape::BoxShape(float width, float height) : PolygonShape({
    Vec2(-width / 2.0f, -height / 2.0f),
    Vec2(width / 2.0f, -height / 2.0f),
    Vec2(width / 2.0f, height / 2.0f),
    Vec2(-width / 2.0f, height / 2.0f)
}), width(width), height(height) {}