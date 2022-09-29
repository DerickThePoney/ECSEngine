#pragma once

namespace ECSEngine
{
class Polygon2D
{
public:
    Polygon2D() { }

    std::vector<vec2>::iterator begin() { return FPoints.begin(); }
    std::vector<vec2>::iterator end() { return FPoints.end(); }
    std::vector<vec2>::const_iterator cbegin() const { return FPoints.cbegin(); }
    std::vector<vec2>::const_iterator cend() const { return FPoints.cend(); }

    std::vector<vec2>::reverse_iterator rbegin() { return FPoints.rbegin(); }
    std::vector<vec2>::reverse_iterator rend() { return FPoints.rend(); }
    std::vector<vec2>::const_reverse_iterator crbegin() const { return FPoints.crbegin(); }
    std::vector<vec2>::const_reverse_iterator crend() const { return FPoints.crend(); }

    void append(const std::vector<vec2>& parPoints) { FPoints.insert(FPoints.end(), parPoints.begin(), parPoints.end()); }
    void push_back(const vec2& parPoint) { FPoints.push_back(parPoint); }
    void reserve(const std::size_t parSize) { FPoints.reserve(parSize); }
    void resize(const std::size_t parSize) { FPoints.resize(parSize); }
    void resize(const std::size_t parSize, const vec2& parDefault) { FPoints.resize(parSize, parDefault); }

    std::size_t size() const { return FPoints.size(); }
    bool empty() const { return FPoints.empty(); }

    void erase(const size_t at) { FPoints.erase(FPoints.begin() + at); }
    void erase(const std::vector<vec2>::iterator at) { FPoints.erase(at); }
    void erase(const std::vector<vec2>::const_iterator at) { FPoints.erase(at); }

    vec2& operator[](const size_t at) { return FPoints[at]; }
    const vec2 operator[](const size_t at) const { return FPoints[at]; }

    std::vector<vec2>& data() { return FPoints; }
    const std::vector<vec2>& data() const { return FPoints; }

    void set_points(const std::vector<vec2>& parPoints) { FPoints = parPoints; }
    void set_points(std::vector<vec2>&& parPoints) { FPoints = parPoints; }

    bool IsClockWise() const;
    Polygon2D Revert() const;

    float Area2Signed() const;
    float AreaSigned() const;

    float Area2() const;
    float Area() const;

    void clear() { FPoints.clear(); }

    template<class Archive>
    void serialize(Archive& ar)
    {
        PROPERTYFIELD(Points, std::vector<vec2>({ vec2(0.f, 0.f), vec2(1.f, 0.f), vec2(0.f, 1.f) }));
    }

private:
    std::vector<vec2> FPoints;
};
} // namespace ECSEngine
