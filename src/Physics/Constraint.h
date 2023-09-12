#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "./Math/Vec2.h"
#include "./Math/MatMN.h"

// Forward declaration
class Body;

class Constraint
{
public:
    Body* a = nullptr;
    Body* b = nullptr;
    
    // The anchor points in local space
    Vec2 aPoint{}, bPoint{};
    
    virtual ~Constraint() = default;
    
    MatMN GetInvMassMatrix() const;
    VecN GetVelocities() const;
    
    virtual void PreSolve(float deltaTime) {}
    virtual void Solve() {}
    virtual void PostSolve() {}
};

class JointConstraint : public Constraint
{
private:
    MatMN jacobian{1, 6};
    VecN cachedLambda{1};
    float bias = 0.0f;
    
public:
    JointConstraint();
    JointConstraint(Body* a, Body* b, const Vec2& anchor);
    
    void PreSolve(float deltaTime) override;
    void Solve() override;
    void PostSolve() override;
};

class PenetrationConstraint : public Constraint
{
private:
    MatMN jacobian{};

};

#endif
