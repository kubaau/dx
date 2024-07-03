#include "Window.hpp"
#include <windowsx.h>
#include <cassert>
#include "MainWndProc.hpp"

Window::Window(InstanceHandle instance_handle, Title title, Width width, Height height)
    : instance_handle(instance_handle), title(title), width(width), height(height)
{
    registerClass();
    create();
}

void Window::registerClass()
{
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance_handle;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = title;

    assert(RegisterClass(&wc) && "RegisterClass failed");
}

void Window::create()
{
    RECT r = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);

    window_handle = CreateWindow(
        title, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, instance_handle, 0);

    assert(window_handle && "CreateWindow failed");
}
