#include "Shape.h"

#include <limits>

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

Vec2 PolygonShape::GetEdge(int index) const
{
    // Get the current and next vertex
    const Vec2& current = worldVertices[index];
    const Vec2& next = worldVertices[(index + 1) % worldVertices.size()];

    // Return the edge
    return next - current;
}

Vec2 PolygonShape::GetNormal(int index) const
{
    // Get the edge vector
    Vec2 edge = GetEdge(index);

    // Return the normal
    return edge.Normal();
}

float PolygonShape::FindMinSeparation(const PolygonShape& other, Vec2& outAxis, Vec2& outPoint) const
{
    float separation = std::numeric_limits<float>::lowest();

    for (int i = 0; i < worldVertices.size(); ++i)
    {
        float minSep = std::numeric_limits<float>::max();
        Vec2 minVertex;
        Vec2 va = worldVertices[i];
        Vec2 normal = GetNormal(i);

        for (int j = 0; j < other.worldVertices.size(); ++j)
        {
            Vec2 vb = other.worldVertices[j];
            float projection = (vb - va).Dot(normal);

            // Keep track of the smallest separation and the min vertex
            if (projection < minSep)
            {
                minSep = projection;
                minVertex = vb;
            }
        }

        // Keep track of the best separation
        if (minSep > separation)
        {
            separation = minSep;
            outAxis = GetEdge(i);
            outPoint = minVertex;
        }
    }

    return separation;
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