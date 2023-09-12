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
    
    virtual void Solve() {}
};

class JointConstraint : public Constraint
{
private:
    MatMN jacobian{};
    
public:
    JointConstraint();
    JointConstraint(Body* a, Body* b, const Vec2& anchor);
    
    void Solve() override;
    
protected:
    VecN SolveGaussSeidel(const MatMN& A, const VecN& b, int iterations = 10) const;
};

class PenetrationConstraint : public Constraint
{
private:
    MatMN jacobian{};

};

#endif
