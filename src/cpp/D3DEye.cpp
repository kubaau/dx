#include "D3DEngine.hpp"
#include "Effects.hpp"

void D3DEngine::initEye()
{
    camera->setPosition(float3{-7.0f, 5.0f, -20.0f});
    camera->lookAt(float3{0.0f, 1.0f, -5.0f});

    updateEye();
}

void D3DEngine::updateEye()
{
    eye_pos = camera->getPosition();

    updateViewMatrix();
    updateEffectEye();
    updateEyeTechniques();
}

void D3DEngine::updateEffectEye()
{
    for (auto& effect : effects)
        effect.second.vector("eye_pos", eye_pos);
}

void D3DEngine::updateEyeTechniques()
{
    for (auto& o : renderables)
        o.second.updateEye(eye_pos);
}

void D3DEngine::updateViewMatrix()
{
    using namespace DirectX;
    const auto camera_position = XMVectorSet(eye_pos.x, eye_pos.y, eye_pos.z, 1.0f);
    const auto camera_target = XMVectorZero();
    const auto camera_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    const auto v = XMMatrixLookAtLH(camera_position, camera_target, camera_up);
    XMStoreFloat4x4(&view, v);
    updateViewProjectionMatrix();
}

void D3DEngine::updateViewProjectionMatrix()
{
    const auto vp = XMLoadFloat4x4(&view) * XMLoadFloat4x4(&projection);
    XMStoreFloat4x4(&view_projection, vp);
}
