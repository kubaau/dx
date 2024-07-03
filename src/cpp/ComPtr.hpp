#pragma once

#include <wrl/client.h>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

template <typename T>
auto& operator*(const ComPtr<T>& com_ptr)
{
    return *com_ptr.Get();
}
