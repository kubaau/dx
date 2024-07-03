#pragma once

#include "Interface.hpp"
#include "WindowsTypedefs.hpp"

class IWindow : public Interface
{
public:
    virtual Width getWidth() const = 0;
    virtual void setWidth(Width) = 0;

    virtual Height getHeight() const = 0;
    virtual void setHeight(Height) = 0;

    virtual Title getTitle() const = 0;

    virtual WindowHandle getHandle() const = 0;
};
