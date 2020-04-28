#pragma once

namespace ECSEngine
{
class Resource;
namespace Rendering
{

class Mesh;

class MeshFileReader
{
public:
    MeshFileReader();
    ~MeshFileReader();

    void ReadMesh(Mesh*& parMesh, const std::string& parFilename);
    void ReadMesh(Mesh*& parMesh, Resource& parResource);
};

} // namespace Rendering
} // namespace ECSEngine
