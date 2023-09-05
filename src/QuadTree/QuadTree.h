#pragma once

#include <cassert>
#include <memory>
#include <stack>
#include <vector>

#include "Rectangle.h"

namespace QuadTree {

template <typename T, typename Data> class Point {
public:
  Vector2<T> position;
  std::shared_ptr<Data> data;

  Point() = default;

  Point(T x, T y) : position(x, y) {}

  Point(T x, T y, const std::shared_ptr<Data> &customData)
      : position(x, y), data(customData) {}

  Point(const Vector2<T> &position, const std::shared_ptr<Data> &customData)
      : position(position), data(customData) {}
};

template <typename T, typename Data> class QuadTree {
private:
  struct Node {
    Rectangle<T> boundary;
    size_t capacity;
    std::vector<Point<T, Data>> points;
    bool divided;

    std::unique_ptr<Node> northeast, northwest, southeast, southwest;

    Node(const Rectangle<T> &boundary, size_t capacity)
        : boundary(boundary), capacity(capacity), divided(false) {}

    bool insert(const Point<T, Data> &point) {
      if (!boundary.contains(point.position.x, point.position.y))
        return false;

      if (points.size() < capacity - 1) {
        points.push_back(point);
        return true;
      }

      if (!divided)
        subdivide();

      return northeast->insert(point) || northwest->insert(point) ||
             southeast->insert(point) || southwest->insert(point);
    }

    void subdivide() {
      T x = boundary.position.x;
      T y = boundary.position.y;
      T w = boundary.size.x / 2;
      T h = boundary.size.y / 2;

      Rectangle<T> ne(x + w, y - h, w, h);
      northeast = std::make_unique<Node>(ne, capacity);
      Rectangle<T> nw(x - w, y - h, w, h);
      northwest = std::make_unique<Node>(nw, capacity);
      Rectangle<T> se(x + w, y + h, w, h);
      southeast = std::make_unique<Node>(se, capacity);
      Rectangle<T> sw(x - w, y + h, w, h);
      southwest = std::make_unique<Node>(sw, capacity);

      divided = true;
    }

    void query(const Rectangle<T> &range, std::vector<Point<T, Data>> &found) {
      if (!boundary.intersects(range))
        return;

      for (const auto &p : points) {
        if (!range.contains(p.position.x, p.position.y))
          continue;

        found.push_back(p);
      }

      if (!divided)
        return;

      northeast->query(range, found);
      northwest->query(range, found);
      southeast->query(range, found);
      southwest->query(range, found);
    }
  };

  std::unique_ptr<Node> root;

public:
  QuadTree() = default;
  explicit QuadTree(const Rectangle<T> &boundary, size_t capacity = 4)
      : root(std::make_unique<Node>(boundary, capacity)) {}

  bool insert(const Point<T, Data> &point) { return root->insert(point); }

  void query(const Rectangle<T> &range, std::vector<Point<T, Data>> &found) {
    root->query(range, found);
  }

  std::vector<Point<T, Data> *> getAllPoints() {
    std::vector<Point<T, Data> *> points;
    std::stack<Node *> nodeStack;
    nodeStack.push(root.get());

    do {
      Node *currentNode = nodeStack.top();
      nodeStack.pop();

      for (Point<T, Data> &p : currentNode->points) {
        points.push_back(&p);
      }

      if (!currentNode->divided)
        continue;

      nodeStack.push(currentNode->northeast.get());
      nodeStack.push(currentNode->northwest.get());
      nodeStack.push(currentNode->southeast.get());
      nodeStack.push(currentNode->southwest.get());
    } while (!nodeStack.empty());

    return points;
  }

  std::vector<Rectangle<T> *> getAllBoundaries() {
    std::vector<Rectangle<T> *> boundaries;
    std::stack<Node *> nodeStack;
    nodeStack.push(root.get());

    do {
      Node *currentNode = nodeStack.top();
      nodeStack.pop();

      boundaries.push_back(&currentNode->boundary);

      if (!currentNode->divided)
        continue;

      nodeStack.push(currentNode->northeast.get());
      nodeStack.push(currentNode->northwest.get());
      nodeStack.push(currentNode->southeast.get());
      nodeStack.push(currentNode->southwest.get());
    } while (!nodeStack.empty());

    return boundaries;
  }
};
} // namespace QuadTree
