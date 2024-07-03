#pragma once

#include "ContainerTypedefs.hpp"
#include "DXTypedefs.hpp"

extern DSStates dss;

void initDSStates(Device&);
namespace DSS
{
    void initMarkStencil(Device&);
    void initDrawReflection(Device&);
    void initNoDoubleBlend(Device&);
    void initDepthWriteOff(Device&);
    void initDepthCounter(Device&);
    void initDepthCounterVisualizer(Device&);
    void initLessEqual(Device&);
} // namespace DSS
