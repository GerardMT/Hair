#include "particle.h"

Particle::Particle() {}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, float mass)
{
    pos_ = pos;
    pos_pre_ = pos;

    vel_ = vel;

    mass_ = mass;
    force_ = glm::vec3(0);
}
