#include "stdafx.h"

#include "TextureDescriptor.h"

namespace ECSEngine
{
namespace Rendering
{
namespace TextureFlags
{
#define DONOTINCLUDELENGTH
#define TEXTURE_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
        name, #name                                                                                                                                                                \
    }
std::map<u64, std::string> TextureFlagToName = {
#include "TextureFlags.inl"
};
#undef TEXTURE_FLAG

#define TEXTURE_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
#name, name                                                                                                                                                                \
    }
std::map<std::string, Type> TextureNameToFlag = {
#include "TextureFlags.inl"
};
#undef TEXTURE_FLAG

#define TEXTURE_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
        name, value                                                                                                                                                                \
    }
std::map<Type, u64> TextureFlagToBGFXFlag = {
#include "TextureFlags.inl"
};
#undef TEXTURE_FLAG

#define TEXTURE_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
        value, name                                                                                                                                                                \
    }
std::map<u64, Type> SamplerBGFXFlagToFlag = {
#include "TextureFlags.inl"
};
#undef TEXTURE_FLAG

#undef DONOTINCLUDELENGTH

u64 ConvertToTextureFlags(const std::vector<std::string>& parFlagList)
{
    u64 res = 0;
    foreachitemconst(flag, parFlagList)
    {
        AssertReleaseMsg(TextureNameToFlag.find(flag) != TextureNameToFlag.end(), (flag + " does not exist!").c_str());
        const Type f = TextureNameToFlag[flag];
        AssertRelease(TextureFlagToBGFXFlag.find(f) != TextureFlagToBGFXFlag.end());
        res |= TextureFlagToBGFXFlag[f];
    }
    return res;
}

void ConvertToTextureFlagsList(const u64 parFlags, std::vector<std::string>& parFlagList)
{
    forrange(i, 0, Type::TEXTURE_FLAGS_LENGTH)
    {
        const Type f = (Type)i;
        AssertRelease(TextureFlagToBGFXFlag.find(f) != TextureFlagToBGFXFlag.end());
        const u64 bgfxFlag = TextureFlagToBGFXFlag[f];
        if (parFlags & bgfxFlag)
        {
            AssertRelease(TextureFlagToName.find(f) != TextureFlagToName.end());
            parFlagList.push_back(TextureFlagToName[f]);
        }
    }
}
} // namespace TextureFlags

namespace SamplerFlags
{

#define DONOTINCLUDELENGTH
#define SAMPLER_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
        name, #name                                                                                                                                                                \
    }
std::map<u64, std::string> SamplerFlagToName = {
#include "SamplerFlags.inl"
};
#undef SAMPLER_FLAG

#define SAMPLER_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
#name, name                                                                                                                                                                \
    }
std::map<std::string, Type> SamplerNameToFlag = {
#include "SamplerFlags.inl"
};
#undef SAMPLER_FLAG

#define SAMPLER_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
        name, value                                                                                                                                                                \
    }
std::map<Type, u64> SamplerFlagToBGFXFlag = {
#include "SamplerFlags.inl"
};
#undef SAMPLER_FLAG

#define SAMPLER_FLAG(name, value)                                                                                                                                                  \
    {                                                                                                                                                                              \
        value, name                                                                                                                                                                \
    }
std::map<u64, Type> SamplerBGFXFlagToFlag = {
#include "SamplerFlags.inl"
};
#undef SAMPLER_FLAG

#undef DONOTINCLUDELENGTH

u64 ConvertToSamplerFlags(const std::vector<std::string>& parFlagList)
{
    u64 res = 0;
    foreachitemconst(flag, parFlagList)
    {
        AssertReleaseMsg(SamplerNameToFlag.find(flag) != SamplerNameToFlag.end(), (flag + " does not exist!").c_str());
        const Type f = SamplerNameToFlag[flag];
        AssertRelease(SamplerFlagToBGFXFlag.find(f) != SamplerFlagToBGFXFlag.end());
        res |= SamplerFlagToBGFXFlag[f];
    }
    return res;
}

void ConvertToSamplerFlagsList(const u64 parFlags, std::vector<std::string>& parFlagList)
{
    forrange(i, 0, Type::SAMPLER_FLAGS_LENGTH)
    {
        const Type f = (Type)i;
        AssertRelease(SamplerFlagToBGFXFlag.find(f) != SamplerFlagToBGFXFlag.end());
        const u64 bgfxFlag = SamplerFlagToBGFXFlag[f];
        if (parFlags & bgfxFlag)
        {
            AssertRelease(SamplerFlagToName.find(f) != SamplerFlagToName.end());
            parFlagList.push_back(SamplerFlagToName[f]);
        }
    }
}
} // namespace SamplerFlags

//----------------------------------------------------------------
//          TextureDescriptor
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(TextureDescriptor);

TextureDescriptor::TextureDescriptor()
    : RefCountedObject()
    , FTextureFile("")
    , FFlags(0)
    , FMipMap(false)
    , FLinear(false)
{
}

TextureDescriptor::TextureDescriptor(const std::string& parTextureFile, const u64 parFlags, const bool parMipMap, const bool parLinear)
    : RefCountedObject()
    , FTextureFile(parTextureFile)
    , FFlags(parFlags)
    , FMipMap(parMipMap)
    , FLinear(parLinear)
{
}

TextureDescriptor::~TextureDescriptor()
{
}

} // namespace Rendering
} // namespace ECSEngine
