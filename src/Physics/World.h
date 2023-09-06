#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "Body.h"
#include "Contact.h"

class World
{
public:
	World() = default;
	World(float gravity);
	~World();

public:
	inline std::vector<Body*>& GetBodies() { return bodies; }
	
	void AddBody(Body* body);
	void RemoveBody(Body* body);
	
	void AddForce(const Vec2& force);
	void AddTorque(float torque);
	
	void Update(float deltaTime);
	
	void CheckCollisions();
	
private:
	float G = 9.8f;
	
	std::vector<Body*> bodies = std::vector<Body*>();
	std::vector<Vec2> forces = std::vector<Vec2>();
	std::vector<float> torques = std::vector<float>();
};


#endif
