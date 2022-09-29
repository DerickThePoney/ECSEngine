#pragma once

namespace ECSEngine
{
namespace Navigation
{
struct NavMeshVertex;
class NavMeshPath
{
public:
    NavMeshPath()
        : FStart(vec2(0.f))
        , FEnd(vec2(0.f))
    {
    }
    NavMeshPath(const vec2 parStart, const vec2 parEnd)
        : FStart(parStart)
        , FEnd(parEnd)
    {
    }
    ~NavMeshPath() { }

    void SetValid(const bool parValid) { FValid = parValid; }
    bool isValid() const;

    std::vector<vec2>::iterator begin() { return FWaypoints.begin(); }
    std::vector<vec2>::iterator end() { return FWaypoints.end(); }
    std::vector<vec2>::const_iterator cbegin() const { return FWaypoints.cbegin(); }
    std::vector<vec2>::const_iterator cend() const { return FWaypoints.cend(); }

    std::vector<vec2>::reverse_iterator rbegin() { return FWaypoints.rbegin(); }
    std::vector<vec2>::reverse_iterator rend() { return FWaypoints.rend(); }
    std::vector<vec2>::const_reverse_iterator crbegin() const { return FWaypoints.crbegin(); }
    std::vector<vec2>::const_reverse_iterator crend() const { return FWaypoints.crend(); }

    std::size_t size() const { return (FStart == FEnd) ? 0 : FWaypoints.size() + 2; }
    std::size_t waypoints_size() const { return FWaypoints.size(); }
    bool empty() const { return FStart == FEnd && FWaypoints.empty(); }

    vec2& operator[](const size_t at) { return FWaypoints[at]; }
    const vec2 operator[](const size_t at) const { return FWaypoints[at]; }

    void reserve(const std::size_t parSize) { FWaypoints.reserve(parSize); }

    void push_back(vec2 parWaypoint);

    vec2 Start() const { return FStart; }
    vec2 End() const { return FEnd; }

#ifdef PERFORM_SECURITY_CHECKS
    void AssertExistsAndNoDoublon(vec2 parWaypoint) const;
#endif

private:
    vec2 FStart = vec2(0.f);
    vec2 FEnd = vec2(0.f);
    std::vector<vec2> FWaypoints;
    bool FValid = false;
};
} // namespace Navigation
} // namespace ECSEngine
