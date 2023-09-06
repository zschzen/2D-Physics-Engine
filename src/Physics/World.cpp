#include "World.h"

#include "Constants.h"
#include "CollisionDetection.h"

World::World(float gravity) : G(-gravity)
{
}

World::~World()
{
	for (auto &body: bodies) {
		delete body;
		body = nullptr;
	}
	bodies.clear();
	forces.clear();
	torques.clear();
}

void World::AddBody(Body *body)
{
	bodies.push_back(body);
}

void World::RemoveBody(Body *body)
{
	auto it = std::find(bodies.begin(), bodies.end(), body);
	if (it != bodies.end()) {
		bodies.erase(it);
	}
}

void World::AddForce(const Vec2 &force)
{
	forces.push_back(force);
}

void World::AddTorque(float torque)
{
	torques.push_back(torque);
}

void World::Update(float deltaTime)
{
	for (auto &body: bodies) {
		if (body->IsStatic()) continue;

		// Apply gravity
		body->AddForce(Vec2(0, body->gravityScale * (G * body->mass * PIXELS_PER_METER)));

		// Apply forces
		for (Vec2 &force: forces) {
			body->AddForce(force);
		}

		// Apply torques
		for (float &torque: torques) {
			body->AddTorque(torque);
		}
	}

	for (auto &body: bodies) {
		body->Update(deltaTime);
		body->isColliding = false;
	}

	// Check collisions
	CheckCollisions();
}

void World::CheckCollisions()
{
	for (int i = 0; i <= bodies.size() - 1; ++i) {
		for (int j = i + 1; j < bodies.size(); ++j) {
			Body *bodyA = bodies[i];
			Body *bodyB = bodies[j];

			Contact contact;
			if (!CollisionDetection::IsColliding(bodyA, bodyB, contact)) continue;
			
			// Resolve collision
			contact.ResolveCollision();

			bodyA->isColliding = true;
			bodyB->isColliding = true;
		}
	}
}
