#pragma once

#include "Shape.h"

namespace QuadTree
{
  template<typename T = float>
  class Rectangle : public Shape<T>
  {
    public:
      Rectangle() : Shape<T>() {}
      Rectangle(float x, float y, float width, float height) : Shape<T>(x, y, width, height) {}
      Rectangle(const Vector2<T>& position, const Vector2<T>& size) : Shape<T>(position, size) {}
      ~Rectangle() = default;
      
      bool contains(T x, T y) const override
      {
        return (x >= this->position.x - this->size.x &&
                x < this->position.x + this->size.x &&
                y >= this->position.y - this->size.y &&
                y < this->position.y + this->size.y);
      }
      
        bool intersects(const Shape<T>& range) const override
        {
          return !(range.position.x - range.size.x > this->position.x + this->size.x ||
                 range.position.x + range.size.x < this->position.x - this->size.x ||
                 range.position.y - range.size.y > this->position.y + this->size.y ||
                 range.position.y + range.size.y < this->position.y - this->size.y);
        }
  };
}
