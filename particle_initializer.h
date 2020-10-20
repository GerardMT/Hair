#ifndef PARTICLEINITIALIZER_H
#define PARTICLEINITIALIZER_H

#include "particle.h"

class ParticleInitializer
{
public:
    virtual ~ParticleInitializer() {};

    virtual void initialize(float dt, Particle &p) = 0;
};

#endif // PARTICLEINITIALIZER_H
