#pragma once

#include "IWindow.hpp"

class Window : public IWindow
{
public:
    Window(InstanceHandle, Title, Width, Height);

    Width getWidth() const override { return width; }
    void setWidth(Width w) override { width = w; }
    Height getHeight() const override { return height; }
    void setHeight(Height h) override { height = h; }
    Title getTitle() const override { return title; }
    WindowHandle getHandle() const override { return window_handle; }

private:
    void registerClass();
    void create();

    InstanceHandle instance_handle;
    Title title;
    Width width;
    Height height;
    WindowHandle window_handle;
};
