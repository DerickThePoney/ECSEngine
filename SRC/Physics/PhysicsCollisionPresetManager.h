#pragma once

namespace ECSEngine
{
namespace Physics
{
struct PhysicsCollisionPreset
{
public:
    std::string FName = "Default";
    u32 FCollisionCategory = 1;
    u32 FCollisionMask = 0;

private:
    std::string GetCategoryName() const;
    std::vector<std::string> GetCollisionMaskNames() const;

    void SetCategoryName(std::string& Category);
    void SetCollisionMaskNames(std::vector<std::string>& Masks);

public:
    SAVE()
    {
        ar("Name", FName);
        std::string Category = GetCategoryName();
        NAMEDPROPERTYFIELD("Category", Category, "Default");
        std::vector<std::string> Masks = GetCollisionMaskNames();
        NAMEDPROPERTYFIELD("CollidesWith", Masks, std::vector<std::string>());
    }

    LOAD()
    {
        PROPERTYFIELD(Name, "Default");
        std::string Category;
        NAMEDPROPERTYFIELD("Category", Category, "Default");
        SetCategoryName(Category);

        std::vector<std::string> Masks;
        NAMEDPROPERTYFIELD("CollidesWith", Masks, std::vector<std::string>());
        SetCollisionMaskNames(Masks);
    }
};

class PhysicsEngine;
class PhysicsCollisionPresetManager
{
public:
    void Initialize(PhysicsEngine* Engine);

    const std::vector<PhysicsCollisionPreset>& CollisionPresets() const { return FCollisionPresets; }
    std::vector<PhysicsCollisionPreset>& CollisionPresets() { return FCollisionPresets; }

private:
    std::vector<PhysicsCollisionPreset> FCollisionPresets;
};
} // namespace Physics
} // namespace ECSEngine
