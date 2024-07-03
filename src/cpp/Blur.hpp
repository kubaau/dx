#pragma once

#include "DXTypedefs.hpp"
#include "IBlur.hpp"

class Blur : public IBlur
{
public:
    void init(Device&, DXGI_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM) override;

    void gaussianWeights(float sigma) override;

    void enable() override { enabled = true; }
    void disable() override { enabled = false; }

    void inPlace(Context&, SRV&, UAV&, unsigned blur_count) override;

    SRV& output() override { return *output_srv; }

    bool isEnabled() const override { return enabled; }

private:
    SRVPtr output_srv;
    UAVPtr output_uav;

    bool enabled{};
};
