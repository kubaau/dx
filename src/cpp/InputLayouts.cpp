#include "InputLayouts.hpp"
#include "Effects.hpp"
#include "GraphicsDebugFlags.hpp"
#include "GraphicsDebugUtils.hpp"
#include <tools/Print.hpp>

#define DEBUG_LAYOUT   \
    if (debug_layouts) \
        print("Created input layout", __FUNCTION__, "");

InputLayouts input_layouts;

void initInputLayouts(Device& device)
{
    using namespace IL;
    initBasic(device);
    initTreePoint(device);
    initCircleToCylinder(device);
    initPositionOnly(device);
    initBasicInstanced(device);
    initPosNormalTexTangent(device);
    initTerrain(device);
    initParticle(device);
}

namespace IL
{
    void initBasic(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC basic[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        D3DX11_PASS_DESC desc;
        effects.at("basic").technique("light1")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(basic, 3, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("basic", il);
        DEBUG_LAYOUT;
    }

    void initTreePoint(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC tree_point[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        D3DX11_PASS_DESC desc;
        effects.at("tree_sprite").technique("light3")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(tree_point, 2, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("tree_point", il);
        DEBUG_LAYOUT;
    }

    void initCircleToCylinder(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC circle_to_cylinder[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        D3DX11_PASS_DESC desc;
        effects.at("circle_to_cylinder").technique("light3")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(circle_to_cylinder, 2, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("circle_to_cylinder", il);
        DEBUG_LAYOUT;
    }

    void initPositionOnly(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC position_only[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        D3DX11_PASS_DESC desc;
        effects.at("tessellation").technique("tessellation_quad")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(position_only, 1, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("position_only", il);
        DEBUG_LAYOUT;
    }

    void initBasicInstanced(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC basic_instanced[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1}};

        D3DX11_PASS_DESC desc;
        effects.at("basic_instanced").technique("light3")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(basic_instanced, 8, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("basic_instanced", il);
        DEBUG_LAYOUT;
    }

    void initPosNormalTexTangent(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC pos_normal_tex_tangent[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        D3DX11_PASS_DESC desc;
        effects.at("normal_map").technique("light3_tex_normal")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(pos_normal_tex_tangent, 4, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("pos_normal_tex_tangent", il);
        DEBUG_LAYOUT;
    }

    void initTerrain(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC terrain[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        D3DX11_PASS_DESC desc;
        effects.at("terrain").technique("light3")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(terrain, 3, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("terrain", il);
        DEBUG_LAYOUT;
    }

    void initParticle(Device& device)
    {
        static const D3D11_INPUT_ELEMENT_DESC particle[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"DELAY", 0, DXGI_FORMAT_R32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        D3DX11_PASS_DESC desc;
        effects.at("fire").technique("stream_out")->GetPassByIndex(0)->GetDesc(&desc);
        InputLayoutPtr il;
        HR(device.CreateInputLayout(particle, 6, desc.pIAInputSignature, desc.IAInputSignatureSize, &il));
        input_layouts.emplace("particle", il);
        DEBUG_LAYOUT;
    }
} // namespace IL
