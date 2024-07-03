#include "SpecialEffectsInit.hpp"
#include "Effects.hpp"
#include "IBlur.hpp"
#include "IWavesCPU.hpp"
#include "IWavesGPU.hpp"

void initWavesCPU(IWavesCPU& waves_cpu)
{
    waves_cpu.init(160, 160, 1.0f, 0.03f, 5.0f, 0.3f);
}

void initWavesGPU(IWavesGPU& waves_gpu, Device& device)
{
    waves_gpu.init(device, 512, 512, 1.0f, 0.03f, 5.0f, 0.3f, DXGI_FORMAT_R32_FLOAT, effects.at("basic"));
}

void initBlur(IBlur& blur)
{
    blur.gaussianWeights(5.0f);
    effects.at("blur").scalar("color_sigma", 1.0f);
    effects.at("blur").scalar("bilateral", false);
}

Fog initFog()
{
    Fog fog;
    fog.color = Colors::silver;
    fog.start = 15.0f;
    fog.range = 175.0f;
    return fog;
}
