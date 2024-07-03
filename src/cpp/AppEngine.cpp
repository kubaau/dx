#include "AppEngine.hpp"
#include "D3DEngine.hpp"
#include <tools/Print.hpp>

constexpr auto exit_on_exception = true;

void AppEngine::run(WindowPtr w, TimerPtr t) try
{
    window = move(w);
    timer = move(t);

    ShowWindow(windowHandle(), SW_SHOW);
    UpdateWindow(windowHandle());

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (not state[paused])
        {
            timer->tick();
            updateFrameStats();
            D3D.run();
        }
        else
        {
            constexpr auto sleep_time_ms = 100;
            Sleep(sleep_time_ms);
        }
    }
}
catch (const exception& e)
{
    print("Uncaught standard exception:", e.what());
    if (exit_on_exception)
        throw;
}
catch (...)
{
    print("Uncaught non-standard exception");
    if (exit_on_exception)
        throw;
}

void AppEngine::pause()
{
    state[paused] = true;
    timer->pause();
}

void AppEngine::unpause()
{
    state[paused] = false;
    timer->unpause();
}

void AppEngine::minimize()
{
    pause();
    state[minimized] = true;
    state[maximized] = false;
}

void AppEngine::maximize()
{
    unpause();
    state[minimized] = false;
    state[maximized] = true;
    D3D.onResize();
}

void AppEngine::restore()
{
    // If user is dragging the resize bars, we do not resize the buffers here because as the user continuously
    // drags the resize bars, a stream of WM_SIZE messages are sent to the window, and it would be pointless
    // (and slow) to resize for each WM_SIZE message received from dragging the resize bars. So instead, we
    // reset after the user is done resizing the window and releases the resize bars, which sends a
    // WM_EXITSIZEMOVE message.
    if (state[resizing])
        return;

    if (state[minimized])
    {
        unpause();
        state[minimized] = false;
    }
    else if (state[maximized])
    {
        unpause();
        state[maximized] = false;
    }
    D3D.onResize();
}

void AppEngine::resizingOn()
{
    pause();
    state[resizing] = true;
}

void AppEngine::resizingOff()
{
    unpause();
    state[resizing] = false;
    D3D.onResize();
}

void AppEngine::mouseDown(ButtonState button_state, Point p)
{
    if (button_state & MK_LBUTTON)
    {
        mouse_pos = p;
        SetCapture(windowHandle());
    }
    else if (button_state & MK_RBUTTON)
        D3D.pick(p);
}

void AppEngine::mouseMove(ButtonState button_state, Point p)
{
    D3D.mouseMove(button_state, p);
}

void AppEngine::updateFrameStats()
{
    ++frame_count;
    static double last_update_tick = 0.0f;

    const auto delta = total() - last_update_tick;
    if (delta >= 1.0)
    {
        updateTitle();

        frame_count = 0;
        last_update_tick += delta;
    }
}

void AppEngine::updateTitle()
{
    const auto milliseconds_per_frame = 1000.0 / frame_count;
    ostringstream outs;
    outs.precision(6);
    outs << title() << " "
         << "FPS: " << frame_count << " "
         << "Frame time: " << milliseconds_per_frame << " ms";
    SetWindowText(windowHandle(), outs.str().c_str());
}
