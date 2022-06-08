#pragma once

#define _CRT_SECURE_NO_WARNINGS

// clang-format off
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <queue>
#include <array>
#include <atomic>
#include <malloc.h>
#include <chrono>
// clang-format on

#include "GLMIncludes.h"
#include "Macros.h"
#include "Profiling.h"

#include <cereal/archives/json.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <fmt/format.h>

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

#include "Assertions.h"
#include "Delegate.h"
#include "GLMSerialization.h"
#include "Types.h"
