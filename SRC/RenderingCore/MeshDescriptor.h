#pragma once
#include "Common/MeshHandle.h"

namespace ECSEngine
{
namespace Rendering
{
class IMesh;
class MeshDescriptor
{
public:
    MeshDescriptor();
    ~MeshDescriptor();

    const MeshHandle CreateMesh();

    template<typename Archive>
    void serialize(Archive& ar)
    {
        ar(NAMEDPROPERTY("MeshFile", FFilename));
    }

private:
    std::string FFilename;
};
} // namespace Rendering
} // namespace ECSEngine