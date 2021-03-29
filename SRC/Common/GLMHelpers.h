#pragma once

namespace glm
{
template<typename T, int size, qualifier Q>
bool isNan(const vec<size, T, Q>& parVector)
{
    glm::vec<size, bool, Q> isNanVec = glm::isnan(parVector);
    bool res = false;
    forrange(i, 0, size) { res = res | isNanVec[(typename glm::vec<size, bool, Q>::length_type)i]; }
    return res;
}
} // namespace glm
