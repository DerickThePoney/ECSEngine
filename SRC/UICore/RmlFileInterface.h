#pragma once
#include "Common/PoolAllocator.h"
#include "Common/ResourceHandle.h"

#include <RmlUi/Core/FileInterface.h>

namespace ECSEngine
{
class Resource;
namespace UI
{
class RmlFileInterface : public Rml::FileInterface
{
public:
    virtual Rml::FileHandle Open(const Rml::String& path) override;
    virtual void Close(Rml::FileHandle file) override;
    virtual size_t Read(void* buffer, size_t size, Rml::FileHandle file) override;
    virtual bool Seek(Rml::FileHandle file, long offset, int origin) override;
    virtual size_t Tell(Rml::FileHandle file) override;
    virtual size_t Length(Rml::FileHandle file) override;

private:
    u32 IsFileAlreadyOpened(const Resource& parName);
    u32 GetNextIndexForNewActiveFile();
    u32 ConvertToIndex(Rml::FileHandle file);

private:
    struct InternalFileHandle
    {
        DECLARE_POOL_ALLOCATED(InternalFileHandle);

    public:
        std::shared_ptr<ResourceHandle> FFileHandle = nullptr;
        u32 FReadPtr = 0;
    };

    std::vector<std::unique_ptr<InternalFileHandle>> FActiveFileHandles;
};
} // namespace UI
} // namespace ECSEngine
