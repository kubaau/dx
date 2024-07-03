#pragma once

#include "ContainerTypedefs.hpp"
#include "DXTypedefs.hpp"
#include "MathTypedefs.hpp"

class Effect;

class Particle
{
public:
    Particle(ID, Effect*, SRV*, SRV*, unsigned, float3) {}

    void reset() {}
    void update() {}
    void draw(const Matrix&) {}

private:
};
