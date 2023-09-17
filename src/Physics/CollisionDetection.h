#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include <vector>

// Forward declaration
struct Contact;
struct Body;
struct PolygonShape;

struct CollisionDetection
{
    static bool IsColliding(Body* a, Body* b, std::vector <Contact> &contacts);
    static bool IsCollidingCircleCircle(Body* a, Body* b, std::vector<Contact> &contacts);
    static bool IsCollidingPolygonPolygon(Body* a, Body* b, std::vector<Contact> &contacts);
    static bool IsCollidingPolygonCircle(Body* polygon, Body* circle, std::vector<Contact> &contacts);
};


#endif
