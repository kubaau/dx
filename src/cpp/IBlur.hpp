#pragma once

#include "DXTypedefs.hpp"
#include "Interface.hpp"

class IBlur : public Interface
{
public:
    virtual void init(Device&, DXGI_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM) = 0;

    virtual void gaussianWeights(float sigma) = 0;

    virtual void enable() = 0;
    virtual void disable() = 0;

    virtual void inPlace(Context&, SRV&, UAV&, unsigned blur_count) = 0;

    virtual SRV& output() = 0;

    virtual bool isEnabled() const = 0;
};
