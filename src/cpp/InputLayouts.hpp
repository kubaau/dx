#pragma once

#include "ContainerTypedefs.hpp"
#include "DXTypedefs.hpp"

extern InputLayouts input_layouts;

void initInputLayouts(Device&);
namespace IL
{
    void initBasic(Device&);
    void initTreePoint(Device&);
    void initCircleToCylinder(Device&);
    void initPositionOnly(Device&);
    void initBasicInstanced(Device&);
    void initPosNormalTexTangent(Device&);
    void initTerrain(Device&);
    void initParticle(Device&);
} // namespace IL
