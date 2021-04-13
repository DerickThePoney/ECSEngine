#pragma once

namespace ECSEngine
{
class Resource;
namespace Rendering
{

class Mesh;
class MeshHandle;

class MeshFileReader
{
public:
    MeshFileReader();
    ~MeshFileReader();

    void ReadMesh(const MeshHandle parHandle, Mesh*& parMesh, const std::string& parFilename);
    void ReadMesh(const MeshHandle parHandle, Mesh*& parMesh, const Resource& parResource);
};

} // namespace Rendering
} // namespace ECSEngine
