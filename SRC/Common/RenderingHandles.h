#pragma once

namespace ECSEngine
{
namespace Rendering
{
namespace HandlesId
{
constexpr u32 InvalidMeshIdHandle = (u32)-1;
constexpr u32 InvalidMaterialInstanceHandle = (u32)-1;
constexpr u32 InvalidTextureHandle = (u32)-1;
} // namespace HandlesId

class MeshHandle
{
public:
    MeshHandle(u32 parMeshId = HandlesId::InvalidMeshIdHandle);

    const u32 GetMeshId() const { return FMeshId; }
    bool IsValid() const;

    bool operator<(const MeshHandle& parOther) { return FMeshId < parOther.FMeshId; }

    operator u32() const { return FMeshId; }

private:
    u32 FMeshId;
};

class MaterialInstanceHandle
{
public:
    MaterialInstanceHandle(u32 parMaterialId = HandlesId::InvalidMaterialInstanceHandle);

    const u32 GetMaterialId() const { return FMaterialInstanceId; }
    bool IsValid() const;

    bool operator<(const MaterialInstanceHandle& parOther) const { return FMaterialInstanceId < parOther.FMaterialInstanceId; }

    operator u32() const { return FMaterialInstanceId; }

private:
    u32 FMaterialInstanceId;
};

class TextureName
{
public:
    TextureName(const std::string& parBankName = "", const std::string& parTextureName = "");
    ~TextureName();

    const std::string& BankName() const { return FBankName; }
    const std::string& Texture() const { return FTexture; }

    bool Valid() const { return !FBankName.empty() && !FTexture.empty(); }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(BankName), PROPERTY(Texture));
    }

private:
    std::string FBankName;
    std::string FTexture;
};

class TextureHandle
{
public:
    TextureHandle(u32 parTextureHandle = HandlesId::InvalidTextureHandle);

    const u32 GetTextureId() const { return FTextureHandleId; }
    bool IsValid() const;

    bool operator<(const TextureHandle& parOther) const { return FTextureHandleId < parOther.FTextureHandleId; }

    operator u32() const { return FTextureHandleId; }

private:
    u32 FTextureHandleId;
};
} // namespace Rendering
} // namespace ECSEngine

namespace std
{
template<>
struct hash<ECSEngine::Rendering::MeshHandle>
{
    std::size_t operator()(const ECSEngine::Rendering::MeshHandle& parHandle) const noexcept
    {
        static hash<u32> hash;
        return hash((u32)parHandle);
    }
};
template<>
struct hash<ECSEngine::Rendering::MaterialInstanceHandle>
{
    std::size_t operator()(const ECSEngine::Rendering::MaterialInstanceHandle& parHandle) const noexcept
    {
        static hash<u32> hash;
        return hash((u32)parHandle);
    }
};
} // namespace std
