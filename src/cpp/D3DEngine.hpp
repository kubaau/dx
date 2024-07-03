#pragma once

#include <memory>
#include <vector>
#include "Color.hpp"
#include "Fog.hpp"
#include "IBlur.hpp"
#include "ICamera.hpp"
#include "IComputeShader.hpp"
#include "IWavesCPU.hpp"
#include "IWavesGPU.hpp"
#include "Mesh.hpp"
#include "Particle.hpp"
#include "Renderable.hpp"
#include "Terrain.hpp"
#include "WindowsTypedefs.hpp"

class D3DEngine
{
public:
    static auto& instance()
    {
        static D3DEngine instance;
        return instance;
    }
#define D3D D3DEngine::instance()

    void run();

    void onResize();
    void pick(Point) {}
    void mouseMove(ButtonState, Point);

private:
    D3DEngine();

    void initDependencies();

    void initDirect3D();

    void initScene();
    void initEye();

    // onResize
    void releaseOldCOMs();
    void updateSwapChain();
    void updateRenderTargetView();
    void updateDepthStencilBuffer();
    void updateDepthStencilView();
    void updateViewport();
    void updateProjectionMatrix();
    void updateOffscreenViews();

    void updateEye();
    void updateViewMatrix();
    void updateViewProjectionMatrix();
    void updateEyeTechniques();

    void updateEffectLights();
    void updateEffectEye();
    void updateEffectFog();

    void updateScene();
    void checkControls();
    void checkCameraControls();
    void animateBolt();
    void generateWave();
    void animateWaterTexture();
    void resetParticles();

    void drawScene();
    void clearViews(RTV&, const Color&);
    void visualizeDepth(const Matrix& vp);
    void visualizeBlur();

    DevicePtr device;
    ContextPtr context;
    SwapChainPtr swap_chain;
    RTVPtr rtv;
    Texture2DPtr depth_stencil_buffer;
    DSVPtr dsv;
    SRVPtr offscreen_srv;
    UAVPtr offscreen_uav;
    RTVPtr offscreen_rtv;

    float4x4 view;
    float4x4 projection;
    float4x4 view_projection;
    Color background_color = Colors::black;

    float3 eye_pos;

    Meshes meshes;
    DirectionalLights dir_lights;
    Textures textures;
    Materials materials;
    Particles particles;
    Renderables renderables;

    unique_ptr<ICamera> camera;
    unique_ptr<IBlur> blur;
    unique_ptr<IWavesCPU> waves_cpu;
    unique_ptr<IWavesGPU> waves_gpu;
    vector<unique_ptr<IComputeShader>> compute_shaders;
    Fog fog;
    enum class DepthVisualizer
    {
        none,
        box,
        quad
    } depth_visualizer{};
    Terrain terrain;
};
