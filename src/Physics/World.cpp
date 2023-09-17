#include "World.h"

#include "Constants.h"
#include "CollisionDetection.h"

#include <algorithm>

World::World(float gravity) : G(-gravity)
{
}

World::~World()
{
	for (auto &body: bodies) {
		delete body;
	}
    for (auto &constraint: constraints) {
        delete constraint;
    }
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

void World::AddConstraint(Constraint *constraint)
{
    constraints.push_back(constraint);
}

void World::RemoveConstraint(Constraint *constraint)
{
    auto it = std::find(constraints.begin(), constraints.end(), constraint);
    if (it != constraints.end()) {
        constraints.erase(it);
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
    // Vector of penetration constraints
    std::vector<PenetrationConstraint> penetrations{};
    
	for (auto& body: bodies) {
		if (body->IsStatic()) continue;
        
		// Apply gravity
		body->AddForce(Vec2(0, body->gravityScale * (G * body->mass * PIXELS_PER_METER)));

		// Apply forces
		for (Vec2& force: forces) {
			body->AddForce(force);
		}

		// Apply torques
		for (float& torque: torques) {
			body->AddTorque(torque);
		}
	}

    // Integrate all the forces
    for (auto& body: bodies) {
        body->IntegrateForces(deltaTime);
    }

    // Check penetrations
    CheckCollisions(penetrations);

    // Solve all constraints
    for (auto& constraint: constraints) {
        constraint->PreSolve(deltaTime);
    }
    for (auto& pConstraint: penetrations) {
        pConstraint.PreSolve(deltaTime);
    }
    for (int i = 0; i < 10; i++) {
        for (auto& constraint: constraints)
            constraint->Solve();
        for (auto& pConstraint: penetrations)
            pConstraint.Solve();
    }
    for (auto& constraint: constraints) {
        constraint->PostSolve();
    }
    for (auto& pConstraint: penetrations) {
        pConstraint.PostSolve();
    }

    // Integrate all the velocities
    for (auto& body: bodies) {
        body->IntegrateVelocities(deltaTime);
    }
}

void World::CheckCollisions(std::vector<PenetrationConstraint> &OutPenetrations)
{
    // Check all the bodies with all other bodies detecting collisions
    for (int i = 0; i <= bodies.size() - 1; i++)
        for (int j = i + 1; j < bodies.size(); j++) {
            Body *a = bodies[i];
            Body *b = bodies[j];

            std::vector<Contact> contacts{};
            if (!CollisionDetection::IsColliding(a, b, contacts)) continue;

            // Resolve the collision
            for (auto &contact: contacts) {
                PenetrationConstraint penetration(contact.a, contact.b, contact.start, contact.end, contact.normal);
                OutPenetrations.push_back(penetration);
            }
        }

}
