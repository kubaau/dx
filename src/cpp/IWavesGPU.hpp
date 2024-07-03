#pragma once

#include "DXTypedefs.hpp"
#include "Interface.hpp"
#include "WindowsTypedefs.hpp"

class Effect;

class IWavesGPU : public Interface
{
public:
    virtual void init(Device&,
                      Width,
                      Height,
                      double spatial_step,
                      double time_step,
                      double speed,
                      double damping,
                      DXGI_FORMAT,
                      Effect&) = 0;

    virtual void disturb(Context&, unsigned m, unsigned n, float magnitude) = 0;
    virtual void update(Context&) = 0;

    virtual Width width() const = 0;
    virtual Height height() const = 0;
    virtual SRV& displacement() const = 0;
};
