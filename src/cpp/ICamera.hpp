#pragma once

#include "Interface.hpp"

#include "MathTypedefs.hpp"

class ICamera : public Interface
{
public:    
    virtual void updateView() = 0;
    
    virtual void pitch(float angle) = 0;
    virtual void yaw(float angle) = 0;
    virtual void lookAt(float3 target) = 0;
    virtual void walk(float distance) = 0;
    virtual void walk(float distance, float height) = 0;
    virtual void strafe(float distance) = 0;
    virtual void strafe(float distance, float height) = 0;

    virtual void setLens(float fov_y, float aspect, float z_near, float z_far) = 0;
    virtual void setPosition(float3) = 0;

    virtual float3 getPosition() const = 0;
    virtual Matrix getVP() const = 0;
};
