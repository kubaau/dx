#pragma once

#include "DXTypedefs.hpp"
#include "Fog.hpp"

void initWavesCPU(class IWavesCPU&);
void initWavesGPU(class IWavesGPU&, Device&);
void initBlur(class IBlur&);
Fog initFog();
