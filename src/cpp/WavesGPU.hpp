#pragma once

#include "IWavesGPU.hpp"

class WavesGPU : public IWavesGPU
{
public:
    void init(Device&,
              Width,
              Height,
              double spatial_step,
              double time_step,
              double speed,
              double damping,
              DXGI_FORMAT,
              Effect&) override;

    void disturb(Context&, unsigned m, unsigned n, float magnitude) override;
    void update(Context&) override;

    unsigned width() const override { return width_; }
    unsigned height() const override { return height_; }
    SRV& displacement() const override { return *curr_srv; }

private:
    unsigned width_;
    unsigned height_;
    double time_step;
    double elapsed{};

    Effect* waves_effect;
    SRVPtr prev_srv;
    SRVPtr curr_srv;
    SRVPtr next_srv;
    UAVPtr prev_uav;
    UAVPtr curr_uav;
    UAVPtr next_uav;
};
