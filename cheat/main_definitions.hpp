#pragma once

#define WIN32_LEAN_AND_MEAN

#include <d3d9.h>
#include <d3dx9.h>

#include <windows.h>
#include <libs.h>
#include <stdio.h>

#include <memory>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>
#include <optional>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define INLINE __forceinline

#ifdef _DEBUG
#define DEBUG_LOG(str, ...) printf(str, __VA_ARGS__)
#else
#define DEBUG_LOG(str, ...)
#endif

#define CONCAT_IMPL(x, y) x##y
#define MARCO_CONCAT(x, y) CONCAT_IMPL( x, y )

#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / __interfaces->global_vars->interval_per_tick))
#define TICKS_TO_TIME(t) (__interfaces->global_vars->interval_per_tick * (t))

#undef min
#undef max