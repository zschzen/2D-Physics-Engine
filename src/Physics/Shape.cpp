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

PolygonShape::PolygonShape(const std::vector<Vec2> &vertices) : vertices(vertices) {}

float PolygonShape::GetMomentOfInertia() const
{
    float numerator = 0.0f;
    float denominator = 0.0f;

    for (int i = 0; i < vertices.size(); i++)
    {
        Vec2 a = vertices[i];
        Vec2 b = vertices[(i + 1) % vertices.size()];

        float cross = a.Cross(b);
        float dot = a.Dot(b);

        numerator += cross * (dot + a.Dot(a) + a.Dot(b));
        denominator += cross;
    }

    return numerator / (6.0f * denominator);
}

// --------------------
// BoxShape
// --------------------

BoxShape::BoxShape(float width, float height) : PolygonShape({
    Vec2(-width / 2.0f, -height / 2.0f),
    Vec2(width / 2.0f, -height / 2.0f),
    Vec2(width / 2.0f, height / 2.0f),
    Vec2(-width / 2.0f, height / 2.0f)
}), width(width), height(height) {}

float BoxShape::GetMomentOfInertia() const
{
    return 0.083333 * (width * width + height * height);
}