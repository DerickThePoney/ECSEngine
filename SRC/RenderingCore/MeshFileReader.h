#pragma once

namespace ECSEngine
{
class Resource;
namespace Rendering
{

class IMesh;

class MeshFileReader
{
public:
    MeshFileReader();
    ~MeshFileReader();

    void ReadMesh(IMesh*& parMesh, const std::string& parFilename);
    void ReadMesh(IMesh*& parMesh, Resource& parResource);
};

} // namespace Rendering
} // namespace ECSEngine
