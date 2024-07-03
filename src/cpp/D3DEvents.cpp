#include "AppEngine.hpp"
#include "BlendStates.hpp"
#include <tools/Contains.hpp>
#include "D3DEngine.hpp"
#include "DepthStencilStates.hpp"
#include "Effects.hpp"
#include <tools/KeysValues.hpp>
#include <tools/Random.hpp>

inline auto isPressed(char key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

void D3DEngine::checkControls()
{
    const auto time_delta_f = APP.deltaF();

    if (isPressed('1'))
        for (auto& o : renderables)
            o.second.renderOption(RenderOption::no_tex);
    if (isPressed('2'))
        for (auto& o : renderables)
            o.second.renderOption(RenderOption::none);
    if (isPressed('3'))
        for (auto& o : renderables)
            o.second.renderOption(RenderOption::fog);
    if (isPressed('4'))
    {
        for (auto& o : renderables)
            o.second.overrideDSS(dss["depth_counter"].Get());
        depth_visualizer = DepthVisualizer::box;
    }
    if (isPressed('5'))
    {
        for (auto& o : renderables)
            o.second.overrideDSS(dss["depth_counter"].Get());
        depth_visualizer = DepthVisualizer::quad;
    }
    if (isPressed('6'))
        for (auto& o : renderables)
        {
            if (o.second.effect == &effects.at("basic"))
                o.second.renderOption(RenderOption::depth_test_blend);
            o.second.overrideDSS(dss["depth_write_off"].Get());
            o.second.overrideBS(bs["additive"].Get());
        }
    if (isPressed('0'))
        for (auto& o : renderables)
        {
            o.second.overrideDSS(dss_default);
            o.second.overrideBS(bs_default);
            depth_visualizer = DepthVisualizer::none;
        }

    if (isPressed('B'))
        blur->enable();
    if (isPressed('N'))
        blur->disable();
    static auto color_sigma = 1.0f;
    if (isPressed('H'))
    {
        effects.at("blur").scalar("color_sigma", color_sigma -= 0.01f);
        print(color_sigma);
    }
    if (isPressed('J'))
    {
        effects.at("blur").scalar("color_sigma", color_sigma += 0.01f);
        print(color_sigma);
    }
    if (isPressed('K'))
        effects.at("blur").scalar("bilateral", false);
    if (isPressed('L'))
        effects.at("blur").scalar("bilateral", true);

    // if (isPressed('M'))
    //    enable_4x_msaa = false;
    // if (isPressed(','))
    //    enable_4x_msaa = true;

    if (contains(renderables, "skull1"))
    {
        if (isPressed(VK_NUMPAD4))
            renderables.at("skull1").translate(float3(-time_delta_f, 0, 0));
        if (isPressed(VK_NUMPAD6))
            renderables.at("skull1").translate(float3(time_delta_f, 0, 0));
        if (isPressed(VK_NUMPAD8))
            renderables.at("skull1").translate(float3(0, 0, time_delta_f));
        if (isPressed(VK_NUMPAD5))
            renderables.at("skull1").translate(float3(0, 0, -time_delta_f));
        if (isPressed(VK_NUMPAD7))
            renderables.at("skull1").translate(float3(0, time_delta_f, 0));
        if (isPressed(VK_NUMPAD9))
            renderables.at("skull1").translate(float3(0, -time_delta_f, 0));
    }

    // if (isPressed('K'))
    //    renderables.at("");
    // if (isPressed('L'))
    //    throw 42;

    if (isPressed('P'))
        resetParticles();

    debug_wireframe = isPressed('R');

    checkCameraControls();
}

void D3DEngine::checkCameraControls()
{
    constexpr auto walk_step_base = 10.0f;
    const auto walk_step = walk_step_base * APP.deltaF();
    //const auto roll_step = pi / 2 * APP.deltaF();

    bool walk_forward = false, walk_back = false, walk_left = false, walk_right = false;

    if (isPressed('W'))
        walk_forward = true;
    if (isPressed('S'))
        walk_back = true;
    if (isPressed('A'))
        walk_left = true;
    if (isPressed('D'))
        walk_right = true;
    const auto walk = walk_forward or walk_back or walk_left or walk_right;

    static bool walk_on_terrain = false, walk_on_tangent = false;
    if (isPressed('Z'))
        walk_on_terrain = true;
    if (isPressed('X'))
        walk_on_terrain = false;
    if (isPressed('C'))
        walk_on_tangent = true;
    if (isPressed('V'))
        walk_on_tangent = false;

    if (walk)
    {
        const auto initial_position = camera->getPosition();
        float initial_height = 0;

        if (walk_on_terrain)
            initial_height = terrain.getHeight(initial_position.x, initial_position.z);

        if (walk_forward and not walk_back)
            camera->walk(walk_step);
        if (not walk_forward and walk_back)
            camera->walk(-walk_step);
        if (walk_left and not walk_right)
            camera->strafe(-walk_step);
        if (not walk_left and walk_right)
            camera->strafe(walk_step);

        if (walk_on_terrain)
        {
            if (walk_on_tangent)
            {
                const auto new_position = camera->getPosition();
                const auto new_height = terrain.getHeight(new_position.x, new_position.z);
                const auto height_diff = abs(new_height - initial_height);
                camera->setPosition(initial_position);

                if (walk_forward && !walk_back)
                    camera->walk(walk_step, height_diff);
                if (!walk_forward && walk_back)
                    camera->walk(-walk_step, height_diff);
                if (walk_left && !walk_right)
                    camera->strafe(-walk_step, height_diff);
                if (!walk_left && walk_right)
                    camera->strafe(walk_step, height_diff);
            }

            auto final_position = camera->getPosition();
            const auto final_height = terrain.getHeight(final_position.x, final_position.z);
            camera->setPosition({final_position.x, final_height + 2, final_position.z});
        }
    }

    static bool random_lookat_pressed = false;
    if (not random_lookat_pressed and isPressed(VK_SHIFT) and isPressed('X'))
    {
        const auto renderable_keys = keys(renderables);
        const auto random_key_index = randomInt(0u, static_cast<unsigned>(renderable_keys.size()) - 1);
        print("Now looking at", renderable_keys[random_key_index]);
        camera->lookAt(renderables.at(renderable_keys[random_key_index]).t);
        random_lookat_pressed = true;
    }
    else if (not(isPressed(VK_SHIFT) and isPressed('X')))
        random_lookat_pressed = false;
}

void D3DEngine::mouseMove(ButtonState bstate, Point p)
{
    if (bstate & MK_LBUTTON)
    {
        using namespace DirectX;

        // Make each pixel correspond to a quarter of a degree.
        const auto dx = XMConvertToRadians(0.25f * (p.x - APP.mousePos().x));
        const auto dy = XMConvertToRadians(0.25f * (p.y - APP.mousePos().y));

        camera->pitch(dy);
        camera->yaw(dx);

        updateEye();
    }
    if (bstate & MK_RBUTTON)
    {
        // Make each pixel correspond to 0.1 unit in the scene.
        //const auto dx = 0.1f * (p.x - APP.mousePos().x);
        //const auto dy = 0.1f * (p.y - APP.mousePos().y);

        updateEye();
    }

    APP.mousePos(p);
}
