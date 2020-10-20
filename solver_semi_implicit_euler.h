#ifndef SOLVERIMPLICITEULER_H
#define SOLVERIMPLICITEULER_H

#include "solver.h"

class SolverSemiImplicitEuler : public Solver
{
public:
    void solve(float dt, Particle &p) const override;
};

#endif // SOLVERIMPLICITEULER_H
