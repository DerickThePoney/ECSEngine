#pragma once
#include <cereal/archives/json.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

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