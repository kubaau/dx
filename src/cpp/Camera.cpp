#include "Camera.hpp"

using namespace DirectX;

void Camera::updateView()
{
    auto r = XMLoadFloat3(&right);
    auto u = XMLoadFloat3(&up);
    auto l = XMLoadFloat3(&look);
    const auto p = XMLoadFloat3(&pos);

    // Keep camera's axes orthogonal to each other and of unit length.
    l = XMVector3Normalize(l);
    u = XMVector3Normalize(XMVector3Cross(l, r));
    // U, L already ortho-normal, so no need to normalize cross product.
    r = XMVector3Cross(u, l);

    // Fill in the view matrix entries.
    const auto x = -XMVectorGetX(XMVector3Dot(p, r));
    const auto y = -XMVectorGetX(XMVector3Dot(p, u));
    const auto z = -XMVectorGetX(XMVector3Dot(p, l));

    XMStoreFloat3(&right, r);
    XMStoreFloat3(&up, u);
    XMStoreFloat3(&look, l);

    view(0, 0) = right.x;
    view(1, 0) = right.y;
    view(2, 0) = right.z;
    view(3, 0) = x;

    view(0, 1) = up.x;
    view(1, 1) = up.y;
    view(2, 1) = up.z;
    view(3, 1) = y;

    view(0, 2) = look.x;
    view(1, 2) = look.y;
    view(2, 2) = look.z;
    view(3, 2) = z;

    view(0, 3) = 0.0f;
    view(1, 3) = 0.0f;
    view(2, 3) = 0.0f;
    view(3, 3) = 1.0f;
}

void Camera::pitch(float angle)
{
    // Rotate up and look vector about the right vector.
    const auto rotation = XMMatrixRotationAxis(XMLoadFloat3(&right), angle);

    XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotation));
    XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), rotation));
}

void Camera::yaw(float angle)
{
    // Rotate the basis vectors about the world y-axis.
    const auto rotation = XMMatrixRotationY(angle);

    XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), rotation));
    XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), rotation));
    XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), rotation));
}

void Camera::lookAt(float3 target) // do this with XMVectorSubtract and XMVector3Cross!
{
    XMStoreFloat3(&look, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&pos))));
    XMStoreFloat3(&right, XMVector3Cross(XMLoadFloat3(&up_default), XMLoadFloat3(&look)));
    XMStoreFloat3(&up, XMVector3Cross(XMLoadFloat3(&look), XMLoadFloat3(&right)));
}

void Camera::setLens(float fy, float a, float zn, float zf)
{
    fov_y = fy;
    aspect = a;
    z_near = zn;
    z_far = zf;

    window_height_near = 2.0f * z_near * tanf(0.5f * fov_y);
    window_height_far = 2.0f * z_far * tanf(0.5f * fov_y);

    const auto p = XMMatrixPerspectiveFovLH(fov_y, aspect, z_near, z_far);
    XMStoreFloat4x4(&projection, p);
}

void Camera::move(float distance, Vector target)
{
    if (distance)
        // position += distance * target
        XMStoreFloat3(&pos, XMVectorMultiplyAdd(XMVectorReplicate(distance), target, XMLoadFloat3(&pos)));
}

void Camera::move(float distance, float3 target, float height, float3 rotation_axis)
{
    const auto angle = atanf(height / distance);
    const auto rotation = XMMatrixRotationAxis(XMLoadFloat3(&rotation_axis), angle);
    move(distance, XMVector3TransformNormal(XMLoadFloat3(&target), rotation));
}
