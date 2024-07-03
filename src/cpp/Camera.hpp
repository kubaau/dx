#pragma once

#include "ICamera.hpp"

const float3 up_default{0, 1, 0};
const float3 look_default{0, 0, 1};
const float3 right_default{1, 0, 0};

class Camera : public ICamera
{
public:
    void updateView() override;

    void pitch(float angle) override;
    void yaw(float angle) override;
    void lookAt(float3 target) override;
    void walk(float distance) override { move(distance, look); }
    void walk(float distance, float height) override { move(distance, look, height, right); }
    void strafe(float distance) override { move(distance, right); }
    void strafe(float distance, float height) override { move(distance, right, height, look); }

    void setLens(float fy, float a, float zn, float zf) override;
    void setPosition(float3 p) override { pos = p; }

    float3 getPosition() const override { return pos; }
    Matrix getVP() const override { return XMMatrixMultiply(XMLoadFloat4x4(&view), XMLoadFloat4x4(&projection)); }

private:
    void move(float distance, float3 target) { move(distance, XMLoadFloat3(&target)); }
    void move(float distance, Vector target);
    void move(float distance, float3 target, float height, float3 rotation_axis);

    float fov_y;
    float aspect;
    float z_near;
    float z_far;
    float window_height_near;
    float window_height_far;
    float4x4 view;
    float4x4 projection;
    float3 pos;
    float3 up = up_default;
    float3 look = look_default;
    float3 right = right_default;
};
