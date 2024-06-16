#include "stdafx.h"

#include "CollisionShape.h"

#include "Application/PropertyDrawer.h"

namespace ECSEngine
{
namespace Physics
{
std::string GetName(ECollisionShape shapeType)
{
    switch (shapeType)
    {
    case ECSEngine::Physics::ECollisionShape::NONE:
        return "NONE";
        break;
    case ECSEngine::Physics::ECollisionShape::BOX:
        return "BOX";
        break;
    case ECSEngine::Physics::ECollisionShape::SPHERE:
        return "SPHERE";
        break;
    default:
        break;
    }

    return "GetName ECOLLISIONSHAPE ERROR";
}

IMPLEMENT_POOL_ALLOCATED(CollisionShape);

CollisionShape CollisionShape::MakeSphere(vec3 Center, float Radius)
{
    CollisionShape shape;
    shape.FShapeType = ECollisionShape::SPHERE;
    shape.FCenter = Center;
    shape.FShapeData.FSphereData.Radius = Radius;
    return shape;
}

CollisionShape CollisionShape::MakeBox(vec3 Center, vec3 Extents)
{
    CollisionShape shape;
    shape.FShapeType = ECollisionShape::BOX;
    shape.FCenter = Center;
    shape.FShapeData.FBoxData.FExtents = Extents;
    return shape;
}

float CollisionShape::ComputeMass(float Density) const
{
    switch (FShapeType)
    {
    case ECollisionShape::BOX:
        return Density * FShapeData.FBoxData.FExtents.x * FShapeData.FBoxData.FExtents.y * FShapeData.FBoxData.FExtents.z;
    case ECollisionShape::SPHERE:
        return 2.f * Pi() * FShapeData.FSphereData.Radius * Density;
    }
}

void CollisionShape::DrawInEditor()
{
    if (ImGui::CollapsingHeader("CollisionShape"))
    {
        ImGui::Indent();
        if (ImGui::BeginCombo("Shape Type", GetName(FShapeType).c_str()))
        {
            {
                bool is_selected = (FShapeType == ECollisionShape::NONE);
                if (ImGui::Selectable(GetName(ECollisionShape::NONE).c_str(), is_selected))
                    FShapeType = ECollisionShape::NONE;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            {
                bool is_selected = (FShapeType == ECollisionShape::BOX);
                if (ImGui::Selectable(GetName(ECollisionShape::BOX).c_str(), is_selected))
                    FShapeType = ECollisionShape::BOX;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            {
                bool is_selected = (FShapeType == ECollisionShape::SPHERE);
                if (ImGui::Selectable(GetName(ECollisionShape::SPHERE).c_str(), is_selected))
                    FShapeType = ECollisionShape::SPHERE;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        EDITOR_PROPERTY_SIMPLE("Offset", FCenter);

        switch (FShapeType)
        {
        case ECSEngine::Physics::ECollisionShape::BOX:
            EDITOR_PROPERTY_WITH_LIMITS("Extents", FShapeData.FBoxData.FExtents, vec3(0.f), vec3(10000.f));
            break;
        case ECSEngine::Physics::ECollisionShape::SPHERE:
            EDITOR_PROPERTY_WITH_LIMITS("Radius", FShapeData.FSphereData.Radius, 0.f, 10000.f);
            break;
        default:
            break;
        }
        ImGui::Unindent();
    }
}

} // namespace Physics
} // namespace ECSEngine
