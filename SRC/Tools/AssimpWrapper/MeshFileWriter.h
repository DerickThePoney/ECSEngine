#pragma once
#include <assimp/scene.h>

namespace ECSEngine
{
namespace Rendering
{

class MeshFileWriter
{
public:
    MeshFileWriter(const std::string& parFilename);
    ~MeshFileWriter();

    void operator<<(const aiScene* parMeshData);

private:
    std::ofstream FOutputStream;
};

} // namespace Rendering
} // namespace ECSEngine
