#pragma once

#include <bitset>
#include <memory>
#include "ITimer.hpp"
#include "IWindow.hpp"
#include <tools/Typedefs.hpp>

class AppEngine
{
    using WindowPtr = unique_ptr<IWindow>;
    using TimerPtr = unique_ptr<ITimer>;

public:
    static auto& instance()
    {
        static AppEngine instance;
        return instance;
    }
#define APP AppEngine::instance()

    void run(WindowPtr, TimerPtr);

    auto width() const { return window->getWidth(); }
    auto widthF() const { return static_cast<float>(window->getWidth()); }
    auto width(Width v) { window->setWidth(v); }
    auto height() const { return window->getHeight(); }
    auto heightF() const { return static_cast<float>(window->getHeight()); }
    auto height(Height v) { window->setHeight(v); }
    auto title() const { return window->getTitle(); }
    auto windowHandle() const { return window->getHandle(); }
    auto aspectRatio() const { return widthF() / heightF(); }

    auto mousePos() const { return mouse_pos; }
    auto mousePos(Point p) { mouse_pos = p; }

    auto delta() const { return timer->getDelta(); }
    auto total() const { return timer->getTotal(); }
    auto deltaF() const { return timer->getDeltaF(); }
    auto totalF() const { return timer->getTotalF(); }

    void pause();
    void unpause();
    void minimize();
    void maximize();
    void restore();
    void resizingOn();
    void resizingOff();

    void mouseDown(ButtonState, Point);
    void mouseUp(ButtonState, Point) { ReleaseCapture(); }
    void mouseMove(ButtonState, Point);

private:
    AppEngine() {}

    void updateFrameStats();
    void updateTitle();

    enum States : u8
    {
        minimized,
        maximized,
        resizing,
        paused,
        state_count
    };
    bitset<state_count> state;

    WindowPtr window;
    Point mouse_pos;
    TimerPtr timer;
    unsigned frame_count;
};
