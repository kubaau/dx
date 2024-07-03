#include "D3DEngine.hpp"
#include "AssetsInit.hpp"
#include <tools/ContainerOperators.hpp>
#include "CustomGeometry.hpp"
#include "D3DInit.hpp"
#include "Effects.hpp"
#include "InputLayouts.hpp"
#include "RenderStates.hpp"
#include "SpecialEffectsInit.hpp"

D3DEngine::D3DEngine()
{
    using namespace RangeOperators;

    initDependencies();

    initDirect3D();

    initEffects(*device);
    initInputLayouts(*device);
    initRenderStates(*device);

    initWavesCPU(*waves_cpu);
    initWavesGPU(*waves_gpu, *device);
    meshes += buildCustomMeshes(*device, *waves_cpu, *waves_gpu);
    meshes += loadFileMeshes(*device);
    dir_lights = loadLights();
    updateEffectLights();
    textures = loadTextures(*device, *context);
    textures += initRandomTextures(*device);
    materials = loadMaterials();
    particles = loadParticles(textures);
    initScene();
    initEye();
    fog = initFog();
    updateEffectFog();
    initBlur(*blur);

    // particles
    // terrain
}

void D3DEngine::initDirect3D()
{
    initContext(device, context);
    initSwapChain(swap_chain, *device);
    onResize();
}

void D3DEngine::updateEffectLights()
{
    for (auto& effect : effects)
        effect.second.variable("dir_lights", dir_lights.front(), static_cast<unsigned>(dir_lights.size()));
}

void D3DEngine::updateEffectFog()
{
    for (auto& effect : effects)
    {
        auto& e = effect.second;
        e.vector("fog_color", fog.color);
        e.scalar("fog_start", fog.start);
        e.scalar("fog_range", fog.range);
    }
}

void D3DEngine::run()
{
    updateScene();
    drawScene();
}
