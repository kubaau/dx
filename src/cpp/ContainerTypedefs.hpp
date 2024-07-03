#pragma once

#include <string>
#include <unordered_map>

using ID = string;

template <typename T>
using Map = unordered_map<ID, T>;
