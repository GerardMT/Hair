#ifndef PARTICLESSYSTEM_H
#define PARTICLESSYSTEM_H

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <QOpenGLShaderProgram>
#include <vector>

#include "collider.h"
#include "force_field.h"
#include "paint_gl.h"
#include "particle.h"
#include "particle_initializer.h"
#include "solver.h"

using namespace std;

class ParticleSystem : public PaintGL
{
public:
    ParticleSystem(Solver &s, ParticleInitializer &i, unsigned int n, float life_time_);

    void nParticles(unsigned int n);

    void addForceField(ForceField &f);

    void addCollider(Collider &c);

    void solver(Solver &s);

    void particleInitializer(ParticleInitializer &i);

    void initialieGL() override;

    void paintGL(float dt, const Camera &camera) override;

    float life_time_;

private:
    vector<Particle> particles_;

    const Solver *solver_;
    vector<ForceField *> force_fields_;
    vector<Collider *> collliders_;

    ParticleInitializer *initializer_;

    QOpenGLShaderProgram program_;

    GLuint vao_;
    GLuint vbo_;
    GLuint tbo_;
    GLuint cdbo_;
};

#endif // PARTICLESSYSTEM_H
