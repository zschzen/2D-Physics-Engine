#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "Body.h"
#include "Contact.h"
#include "Constraint.h"

class World
{
public:
	World() = default;
	World(float gravity);
	~World();

public:
	inline std::vector<Body*>& GetBodies() { return bodies; }
    inline std::vector<Constraint*>& GetConstraints() { return constraints; }
	
	void AddBody(Body* body);
    void AddConstraint(Constraint* constraint);
    void RemoveConstraint(Constraint* constraint);
	void RemoveBody(Body* body);
	void AddForce(const Vec2& force);
	void AddTorque(float torque);
	
	void Update(float deltaTime);
	
	void CheckCollisions(std::vector<PenetrationConstraint> &OutPenetrations);
	
private:
	float G = 9.8f;
	
	std::vector<Body*> bodies = std::vector<Body*>();
    std::vector<Constraint*> constraints = std::vector<Constraint*>();
	std::vector<Vec2> forces = std::vector<Vec2>();
	std::vector<float> torques = std::vector<float>();
};


#endif
