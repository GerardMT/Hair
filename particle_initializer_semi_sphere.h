#ifndef PARTICLEINITIALIZERSEMISPHERE_H
#define PARTICLEINITIALIZERSEMISPHERE_H

#include "particle_initializer.h"

class ParticleInitializerSemiSphere : public ParticleInitializer
{
public:
    ParticleInitializerSemiSphere(glm::vec3 pos_, float speed, float radius);

    void initialize(float dt, Particle &p) override;

    glm::vec3 pos_;

    float speed_;
    float radius_;

    float mass_min_ = 0.1;
    float mass_max_ = 0.5;
};

#endif // PARTICLEINITIALIZERSEMISPHERE_H
