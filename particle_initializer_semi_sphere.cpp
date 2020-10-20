#include "particle_initializer_semi_sphere.h"

ParticleInitializerSemiSphere::ParticleInitializerSemiSphere(glm::vec3 pos, float speed, float radius)
{
    pos_ = pos;
    speed_ = speed;
    radius_ = radius;
}

void ParticleInitializerSemiSphere::initialize(float dt, Particle &p)
{
    float i = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (M_PI / 2.0)));
    float a = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0 * M_PI)));

    glm::vec3 s;
    s.x = sin(i) * cos(a);
    s.y = cos(i);
    s.z = sin(i) * sin(a);

    p.pos_pre_ = radius_ * s;
    p.vel_ = speed_ * s;

    p.mass_ = mass_min_ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (mass_max_ - mass_min_)));

    p.pos_ = p.pos_pre_ + dt * p.vel_;
}
