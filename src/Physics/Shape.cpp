#include "Shape.h"

// --------------------
// CircleShape
// --------------------

CircleShape::CircleShape(float radius) : radius(radius) {}

float CircleShape::GetMomentOfInertia() const
{
    return 0.5f * (radius * radius);
}

// --------------------
// PolygonShape
// --------------------

PolygonShape::PolygonShape(const std::vector<Vec2> &vertices) : localVertices(vertices), worldVertices(vertices)
{
    // Resize the world vertices to match the number of local vertices
    worldVertices.resize(localVertices.size());
}

float PolygonShape::GetMomentOfInertia() const
{
    return 0.0f;
}

void PolygonShape::UpdateVertices(float angle, const Vec2& position)
{
    // Loop all the vertices, transforming from local to world space
    for (int i = 0; i < localVertices.size(); i++) {
        // First rotate, then we translate
        worldVertices[i] = localVertices[i].Rotate(angle);
        worldVertices[i] += position;
    }
}

// --------------------
// BoxShape
// --------------------

BoxShape::BoxShape(float width, float height) : PolygonShape({
    Vec2(-width / 2.0f, -height / 2.0f),
    Vec2(+width / 2.0f, -height / 2.0f),
    Vec2(+width / 2.0f, +height / 2.0f),
    Vec2(-width / 2.0f, +height / 2.0f)
}), width(width), height(height) {}

float BoxShape::GetMomentOfInertia() const
{
    return 0.083333 * (width * width + height * height);
}