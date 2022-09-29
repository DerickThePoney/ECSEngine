#pragma once

#define WITH_GLM 0

#if WITH_GLM
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_ALIGNED_GENTYPES
#define GLM_FORCE_INTRINSICS
#define GLM_FORCE_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_RADIANS
#define GLM_FORCE_INLINE
#define GLM_FORCE_LEFT_HANDED

#define GLM_PRINT_EXTENSIONS 0

#if defined(_DEBUG) && GLM_PRINT_EXTENSIONS
#define GLM_FORCE_MESSAGES
#endif

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#else
#include "Math/Vector.h"
#include "Math/VectorUtils.h"
#include "Math/Matrix.h"
#include "Math/MatrixUtils.h"
#include "Math/Quaternion.h"
#include "Math/QuaternionUtils.h"
#include "Math/MathSerialization.h"
#include "Math/MathFunctions.h"
#endif