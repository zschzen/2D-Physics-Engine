#include "Particle.h"

Particle::Particle(float x, float y, float mass)
{
    this->position = Vec2(x, y);
    this->mass = mass;
}

Particle::~Particle()
{

}