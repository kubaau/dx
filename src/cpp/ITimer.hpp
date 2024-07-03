#pragma once

#include "Interface.hpp"

class ITimer : public Interface
{
public:
    virtual void tick() = 0;
    virtual void pause() = 0;
    virtual void unpause() = 0;

    virtual double getDelta() const = 0;
    virtual double getTotal() const = 0;
    virtual float getDeltaF() const = 0;
    virtual float getTotalF() const = 0;
};