#pragma once

namespace ECSEngine
{
namespace Rendering
{
namespace MeshHandleId
{
constexpr u32 InvalidMeshIdHandle = (u32)-1;
}

class MeshHandle
{
public:
    MeshHandle(u32 parMeshId = MeshHandleId::InvalidMeshIdHandle);

    const u32 GetMeshId() const { return FMeshId; }
    bool IsValid() const;

private:
    u32 FMeshId;
};
} // namespace Rendering
} // namespace ECSEngine
