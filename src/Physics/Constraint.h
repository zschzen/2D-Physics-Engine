#ifndef CONSTRAINT_H
#define CONSTRAINT_H

// Forward declaration
class Body;

class Constraint {
public:
    Body* a = nullptr;
    Body* b = nullptr;
    
    void Solve();
};


#endif
