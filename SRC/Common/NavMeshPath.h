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
        : FStart(glm::vec2(0.f))
        , FEnd(glm::vec2(0.f))
    {
    }
    NavMeshPath(const glm::vec2 parStart, const glm::vec2 parEnd)
        : FStart(parStart)
        , FEnd(parEnd)
    {
    }
    ~NavMeshPath() { }

    void SetValid(const bool parValid) { FValid = parValid; }
    bool isValid() const;

    std::vector<NavMeshVertex*>::iterator begin() { return FWaypoints.begin(); }
    std::vector<NavMeshVertex*>::iterator end() { return FWaypoints.end(); }
    std::vector<NavMeshVertex*>::const_iterator cbegin() const { return FWaypoints.cbegin(); }
    std::vector<NavMeshVertex*>::const_iterator cend() const { return FWaypoints.cend(); }

    std::vector<NavMeshVertex*>::reverse_iterator rbegin() { return FWaypoints.rbegin(); }
    std::vector<NavMeshVertex*>::reverse_iterator rend() { return FWaypoints.rend(); }
    std::vector<NavMeshVertex*>::const_reverse_iterator crbegin() const { return FWaypoints.crbegin(); }
    std::vector<NavMeshVertex*>::const_reverse_iterator crend() const { return FWaypoints.crend(); }

    std::size_t size() const { return (FStart == FEnd) ? 0 : FWaypoints.size() + 2; }
    std::size_t waypoints_size() const { return FWaypoints.size(); }
    bool empty() const { return FStart == FEnd && FWaypoints.empty(); }

    NavMeshVertex*& operator[](const size_t at) { return FWaypoints[at]; }
    const NavMeshVertex* operator[](const size_t at) const { return FWaypoints[at]; }

    void reserve(const std::size_t parSize) { FWaypoints.reserve(parSize); }

    void push_back(NavMeshVertex* parVertex);

    glm::vec2 Start() const { return FStart; }
    glm::vec2 End() const { return FEnd; }

#ifdef PERFORM_SECURITY_CHECKS
    void AssertExistsAndNoDoublon(NavMeshVertex* parVertex) const;
#endif

private:
    glm::vec2 FStart = glm::vec2(0.f);
    glm::vec2 FEnd = glm::vec2(0.f);
    std::vector<NavMeshVertex*> FWaypoints;
    bool FValid = false;
};
} // namespace Navigation
} // namespace ECSEngine
