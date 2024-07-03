#include "Blur.hpp"
#include "Camera.hpp"
#include "ComputeShaderVecAdd.hpp"
#include "ComputeShaderVecLength.hpp"
#include "D3DEngine.hpp"
#include "WavesCPU.hpp"
#include "WavesGPU.hpp"

void D3DEngine::initDependencies()
{
    using namespace std;

    camera = make_unique<Camera>();
    blur = make_unique<Blur>();
    waves_cpu = make_unique<WavesCPU>();
    waves_gpu = make_unique<WavesGPU>();

    compute_shaders.push_back(make_unique<ComputeShaderVecAdd>());
    compute_shaders.push_back(make_unique<ComputeShaderVecLength>());
}
