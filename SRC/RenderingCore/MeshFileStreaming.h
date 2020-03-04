#pragma once

namespace ECSEngine
{
namespace Rendering
{
struct MeshData
{
};

#ifdef WITH_ASSETS_GENERATION
class MeshFileWriter
{
public:
    MeshFileWriter(const std::string& parFilename);
    ~MeshFileWriter();

    void operator<<(const void* parMeshData);

private:
    std::ofstream FOutputStream;
};
#endif
} // namespace Rendering
} // namespace ECSEngine