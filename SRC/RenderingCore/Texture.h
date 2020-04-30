#pragma once
#include "Common/PoolAllocator.h"
#include "Common/RefCountedObject.h"

namespace ECSEngine
{
namespace Rendering
{

// TEXTURE AND SAMPLER FLAGS MUST BE DEALT WITH DIFFERENTLY --> NEEDS AN ASSOCIATION WITH THEIR BGFXCOUNTER PART PLEASE
namespace TextureFlags
{
enum Type
{
#define TEXTURE_FLAG(name, value) name
#include "TextureFlags.inl"
#undef TEXTURE_FLAG
};

u64 ConvertToTextureFlags(const std::vector<std::string>& parFlagList);
void ConvertToTextureFlagsList(const u64 parFlags, std::vector<std::string>& parFlagList);
} // namespace TextureFlags

namespace SamplerFlags
{
enum Type
{
#define SAMPLER_FLAG(name, value) name
#include "SamplerFlags.inl"
#undef SAMPLER_FLAG
};
u64 ConvertToSamplerFlags(const std::vector<std::string>& parFlagList);
void ConvertToSamplerFlagsList(const u64 parFlags, std::vector<std::string>& parFlagList);
} // namespace SamplerFlags

//----------------------------------------------------------------
//          TextureDescriptor
//----------------------------------------------------------------
class TextureDescriptor : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(TextureDescriptor);

public:
    TextureDescriptor();
    TextureDescriptor(const std::string& parTextureFile, const u64 parFlags, const bool parMipMap);
    ~TextureDescriptor();

    const std::string& TextureFile() const { return FTextureFile; }
    u64 Flags() const { return FFlags; }
    bool MipMaps() const { return FMipMap; }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(PROPERTY(TextureFile), PROPERTY(MipMap), PROPERTY(TextureFormat));

        std::vector<std::string> textureFlags, samplerFlags;
        ar(NAMEDPROPERTY("TextureFlags", textureFlags), NAMEDPROPERTY("SamplerFlags", samplerFlags));

        FFlags = 0;
        FFlags |= TextureFlags::ConvertToTextureFlags(textureFlags);
        FFlags |= SamplerFlags::ConvertToSamplerFlags(samplerFlags);
    }

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(PROPERTY(TextureFile), PROPERTY(MipMap), PROPERTY(TextureFormat));

        std::vector<std::string> textureFlags, samplerFlags;
        TextureFlags::ConvertToTextureFlagsList(FFlags, textureFlags);
        SamplerFlags::ConvertToSamplerFlagsList(FFlags, samplerFlags);

        ar(NAMEDPROPERTY("TextureFlags", textureFlags), NAMEDPROPERTY("SamplerFlags", samplerFlags));
    }

protected:
#ifdef PERFORM_SECURITY_CHECKS
    bool CheckTextureDescriptor();
#endif

private:
    std::string FTextureFile;
    u64 FFlags;
    u32 FTextureFormat;
    bool FMipMap;
};

//----------------------------------------------------------------
//          Texture
//----------------------------------------------------------------
class Texture : public RefCountedObject
{
    DECLARE_POOL_ALLOCATED(Texture);

public:
    Texture();
    ~Texture();

private:
};
} // namespace Rendering
} // namespace ECSEngine
