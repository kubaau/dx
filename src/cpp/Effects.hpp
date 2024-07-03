#pragma once

#include "Effect.hpp"
#include "EffectTypedefs.hpp"

extern Effects effects;

void initEffects(Device&);
namespace FX
{
    void initBasic(Device&);
    void initTreeSprite(Device&);
    void initCircleToCylinder(Device&);
    void initIcosahedron(Device&);
    void initComputeMath(Device&);
    void initBlur(Device&);
    void initWaves(Device&);
    void initTessellation(Device&);
    void initBasicInstanced(Device&);
    void initSky(Device&);
    void initNormalMap(Device&);
    void initDisplacementMap(Device&);
    void initWater(Device&);
    void initTerrain(Device&);
    void initParticle(Device&);
    void initShadowMap(Device&);
    void initDebugTexture(Device&);
} // namespace FX
