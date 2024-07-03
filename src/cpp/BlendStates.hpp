#pragma once

#include "ContainerTypedefs.hpp"
#include "DXTypedefs.hpp"

extern BStates bs;

void initBStates(Device&);
namespace BS
{
    void initAlphaToCoverage(Device&);
    void initTransparent(Device&);
    void initNoRenderTargetWrite(Device&);
    void initAdditive(Device&);
} // namespace BS
