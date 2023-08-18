#ifndef SHAPE_H
#define SHAPE_H

#pragma once

#include <vector>
#include "Vec2.h"

enum class ShapeType {
    CIRCLE,
    POLYGON,
    BOX
};

struct Shape {
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual Shape *Clone() const = 0;
    virtual float GetMomentOfInertia() const = 0;
};

struct CircleShape : public Shape {
    float radius;

    CircleShape(float radius);

    virtual ~CircleShape() = default;

    inline ShapeType GetType() const override { return ShapeType::CIRCLE; }

    inline Shape *Clone() const override { return new CircleShape(*this); }
    
    inline float GetMomentOfInertia() const override;
};

struct PolygonShape : public Shape {
    std::vector<Vec2> vertices;

    PolygonShape(const std::vector<Vec2> &vertices);

    virtual ~PolygonShape() = default;

    inline ShapeType GetType() const override { return ShapeType::POLYGON; }

    inline Shape *Clone() const override { return new PolygonShape(*this); }
    
    inline float GetMomentOfInertia() const override;
};

struct BoxShape : public PolygonShape {
    float width, height;

    BoxShape(float width, float height);

    virtual ~BoxShape() = default;

    inline ShapeType GetType() const override { return ShapeType::BOX; }

    inline Shape *Clone() const override { return new BoxShape(*this); }
    
    inline float GetMomentOfInertia() const override;
};

#endif
