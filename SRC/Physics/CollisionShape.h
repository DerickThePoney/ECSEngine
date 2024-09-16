#pragma once

namespace ECSEngine
{
namespace Physics
{
enum class ECollisionShape : u8
{
    NONE,
    BOX,
    SPHERE
};

std::string GetName(ECollisionShape shapeType);

struct CollisionShape
{
    DECLARE_POOL_ALLOCATED(CollisionShape)
public:
    CollisionShape() = default;

    static CollisionShape MakeSphere(vec3 Center, float Radius);
    static CollisionShape MakeBox(vec3 Center, vec3 Extents);

    float ComputeMass(float Density) const;
    mat3 ComputeInertiaTensor(float Mass) const;
    AABB3f ComputeAABB(const mat4& Transform) const;

    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(ShapeType, ECollisionShape::NONE);
        PROPERTYFIELD(Center, vec3(0.f));
        PROPERTYFIELD(ShapeData, ShapeData());
    }

private:
    ECollisionShape FShapeType = ECollisionShape::NONE;
    vec3 FCenter = vec3(0.f);

    struct BoxData
    {
        vec3 FExtents = vec3(0.f);
    };

    struct SphereData
    {
        float Radius = 0.f;
    };

    union ShapeData
    {
        SERIALIZE() { NAMEDPROPERTYFIELD("Data", FBoxData.FExtents, vec3(0.f)); }

        BoxData FBoxData;
        SphereData FSphereData;

        ShapeData()
            : FBoxData()
            , FSphereData()
        {
        }
    };

    ShapeData FShapeData;
};

} // namespace Physics
} // namespace ECSEngine