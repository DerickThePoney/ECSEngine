#pragma once
#define _ITERATOR_DEBUG_LEVEL 0

#include "Macros.h"
// clang-format off
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <array>
#include <thread>
#include <atomic>
// clang-format on

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_INLINE
#ifdef _DEBUG
#define GLM_FORCE_MESSAGES
#endif
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/ext/vector_uint2.hpp>

#include "Types.h"

#include <xmmintrin.h>

#include "Assertions.h"
