#include "Shape.h"

#include <limits>

#include <iostream>

// --------------------
// CircleShape
// --------------------

CircleShape::CircleShape(float radius) : radius(radius) {}

float CircleShape::GetMomentOfInertia() const
{
    return 0.5f * (radius * radius);
}

void CircleShape::UpdateVertices(float angle, const Vec2& position)
{
	// Do nothing
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

float PolygonShape::FindMinSeparation(const PolygonShape& other, int &outIndexReferenceEdge, Vec2& outSupportPoint) const
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
            outIndexReferenceEdge = i;
            outSupportPoint = minVertex;
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

int PolygonShape::FindIncidentEdge(const Vec2 &normal) const {
    int indexIncidentEdge = 0;
    float minDot = std::numeric_limits<float>::max();
    for (int i = 0; i < this->worldVertices.size(); ++i) {
        auto edgeNormal = this->GetEdge(i).Normal();
        auto projection = edgeNormal.Dot(normal);
        if (projection < minDot) {
            minDot = projection;
            indexIncidentEdge = i;
        }
    }
    return indexIncidentEdge;
}

int PolygonShape::ClipSegmentToLine(const std::vector<Vec2> &contactsIn, std::vector<Vec2> &contactsOut, const Vec2 &c0,
                                    const Vec2 &c1) const {
    // Start with no output points
    int numOut = 0;

    // Calculate the distance of end points to the line
    Vec2 normal = (c1 - c0).Normalize();
    float dist0 = (contactsIn[0] - c0).Cross(normal);
    float dist1 = (contactsIn[1] - c0).Cross(normal);

    // If the points are behind the plane
    if (dist0 <= 0)
        contactsOut[numOut++] = contactsIn[0];
    if (dist1 <= 0)
        contactsOut[numOut++] = contactsIn[1];

    // If the points are on different sides of the plane (one distance is negative and the other is positive)
    if (dist0 * dist1 < 0) {
        float totalDist = dist0 - dist1;

        // Find the intersection using linear interpolation: lerp(start,end) => start + t*(end-start)
        float t = dist0 / (totalDist);
        Vec2 contact = contactsIn[0] + (contactsIn[1] - contactsIn[0]) * t;
        contactsOut[numOut] = contact;
        numOut++;
    }
    return numOut;
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