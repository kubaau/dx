#pragma once

#include "ContainerTypedefs.hpp"
#include "DXTypedefs.hpp"

extern RStates rs;

void initRStates(Device&);
namespace RS
{
    void initWireframe(Device&);
    void initWireframeNoCull(Device&);
    void initNoCull(Device&);
    void initCullClockwise(Device&);
    void initWireframeCullClockwise(Device&);
    void initScissor(Device&);
} // namespace RS
