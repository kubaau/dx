#pragma once

#include "ITimer.hpp"
#include <tools/Timer.hpp>

class AppTimer : public ITimer
{
public:
    void tick() override { timer.tick(); }
    void pause() override { timer.pause(); }
    void unpause() override { timer.unpause(); }

    double getDelta() const override { return timer.delta(); }
    double getTotal() const override { return timer.total(); }
    float getDeltaF() const override { return timer.deltaF(); }
    float getTotalF() const override { return timer.totalF(); }

private:
    Timer timer;
};