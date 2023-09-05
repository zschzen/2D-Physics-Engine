#pragma once

#include "Vec2.h"

namespace QuadTree
{
    template<typename T = float>
    class Shape {
        public:
          Shape() : position(), size() {}
          Shape(T x, T y, T width, T height) : position(x, y), size(width, height) {}
          Shape(const Vector2<T>& position, const Vector2<T>& size) : position(position), size(size) {}

                virtual ~Shape() = default;
                virtual bool contains(T x, T y) const = 0;
                virtual bool intersects(const Shape & range) const = 0;

        public:
                Vector2<T> position;
                Vector2<T> size;
    };
}
