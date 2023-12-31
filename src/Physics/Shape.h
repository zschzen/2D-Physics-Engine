#ifndef SHAPE_H
#define SHAPE_H

#pragma once

#include <vector>
#include "Math/Vec2.h"

enum class ShapeType {
    CIRCLE,
    POLYGON,
    BOX
};

struct Shape {
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual Shape *Clone() const = 0;
	virtual void UpdateVertices(float angle, const Vec2& position) = 0;
    virtual float GetMomentOfInertia() const = 0;
};

struct CircleShape : public Shape {
    CircleShape(float radius);
    virtual ~CircleShape() = default;

    ShapeType GetType() const override { return ShapeType::CIRCLE; }
    Shape *Clone() const override { return new CircleShape(*this); }
	void UpdateVertices(float angle, const Vec2& position) override;
    float GetMomentOfInertia() const override;

    float radius;
};

struct PolygonShape : public Shape {
    std::vector<Vec2> localVertices;
    std::vector<Vec2> worldVertices;

    PolygonShape(const std::vector<Vec2> &vertices);
    virtual ~PolygonShape() = default;

    ShapeType GetType() const override { return ShapeType::POLYGON; }
    Shape *Clone() const override { return new PolygonShape(*this); }
    float GetMomentOfInertia() const override;

    // Get the edge
    Vec2 GetEdge(int index) const;

    // Get the perpendicular of the edge
    Vec2 GetNormal(int index) const;

    // Get minimum sepration between polygons
    float FindMinSeparation(const PolygonShape& other, int &outIndexReferenceEdge, Vec2& outSupportPoint) const;

    // Function to rotate and translate the polygon vertices from "local space" to "world space."
    void UpdateVertices(float angle, const Vec2& position) override;

    // Find the incident edge of the polygon based on the reference edge normal
    int FindIncidentEdge(const Vec2 &normal) const;

    // Clip the polygon against the edge
    int ClipSegmentToLine(const std::vector<Vec2>& contactsIn, std::vector<Vec2>& contactsOut, const Vec2& c0, const Vec2& c1) const;
};

struct BoxShape : public PolygonShape {
    float width, height;

    BoxShape(float width, float height);
    virtual ~BoxShape() = default;

    ShapeType GetType() const override { return ShapeType::BOX; }
    Shape *Clone() const override { return new BoxShape(*this); }
    float GetMomentOfInertia() const override;
};

#endif
