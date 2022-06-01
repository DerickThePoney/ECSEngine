#pragma once

#define _CRT_SECURE_NO_WARNINGS

// clang-format off
#include <set>
#include <string>
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <array>
#include <thread>
#include <atomic>
#include <mutex>
#include <malloc.h>
#include <chrono>
// clang-format on

#include <fmt/format.h>

#include "Macros.h"

#include "Profiling.h"

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
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vec_swizzle.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/access.hpp>
#include <cereal/types/polymorphic.hpp>

#define PROPERTY(P) cereal::make_nvp(#P, F##P)
#define NAMEDPROPERTY(N, P) cereal::make_nvp(N, P)
#define PROPERTYFIELD(P, DEFAULT)                                                                                                                                                  \
    try                                                                                                                                                                            \
    {                                                                                                                                                                              \
        ar(PROPERTY(P));                                                                                                                                                           \
    }                                                                                                                                                                              \
    catch (std::runtime_error e)                                                                                                                                                   \
    {                                                                                                                                                                              \
        F##P = DEFAULT;                                                                                                                                                            \
    }
#define NAMEDPROPERTYFIELD(N, P, DEFAULT)                                                                                                                                          \
    try                                                                                                                                                                            \
    {                                                                                                                                                                              \
        ar(NAMEDPROPERTY(N, P));                                                                                                                                                   \
    }                                                                                                                                                                              \
    catch (std::runtime_error e)                                                                                                                                                   \
    {                                                                                                                                                                              \
        P = DEFAULT;                                                                                                                                                               \
    }

#define SERIALIZE()                                                                                                                                                                \
    template<class Archive>                                                                                                                                                        \
    void serialize(Archive& ar)

#define LOAD()                                                                                                                                                                     \
    template<class Archive>                                                                                                                                                        \
    void load(Archive& ar)

#define SAVE()                                                                                                                                                                     \
    template<class Archive>                                                                                                                                                        \
    void save(Archive& ar) const

#include "GLMSerialization.h"

#include "Types.h"

#include <xmmintrin.h>

#include "Assertions.h"

#include "Delegate.h"
