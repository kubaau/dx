#include "RenderStates.hpp"
#include "BlendStates.hpp"
#include "DepthStencilStates.hpp"
#include "RasterizerStates.hpp"

void initRenderStates(Device& device)
{
    initRStates(device);
    initBStates(device);
    initDSStates(device);
}
