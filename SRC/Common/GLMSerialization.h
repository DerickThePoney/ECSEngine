#pragma once

namespace glm
{
template<class Archive>
void serialize(Archive& ar, glm::vec2& v)
{
    ar(NAMEDPROPERTY("x", v.x), NAMEDPROPERTY("y", v.y));
}

template<class Archive>
void serialize(Archive& ar, glm::vec3& v)
{
    ar(NAMEDPROPERTY("x", v.x), NAMEDPROPERTY("y", v.y), NAMEDPROPERTY("z", v.z));
}

template<class Archive>
void serialize(Archive& ar, glm::vec4& v)
{
    ar(NAMEDPROPERTY("x", v.x), NAMEDPROPERTY("y", v.y), NAMEDPROPERTY("z", v.z), NAMEDPROPERTY("w", v.w));
}

template<class Archive>
void serialize(Archive& ar, glm::quat& q)
{
    ar(NAMEDPROPERTY("x", q.x), NAMEDPROPERTY("y", q.y), NAMEDPROPERTY("z", q.z), NAMEDPROPERTY("w", q.w));
}
} // namespace glm