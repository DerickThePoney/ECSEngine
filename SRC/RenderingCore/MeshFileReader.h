#pragma once

namespace ECSEngine
{
namespace Rendering
{

class IMesh;

class MeshFileReader
{
public:
    MeshFileReader(const std::string& parFilename);
    ~MeshFileReader();

    void operator>>(IMesh*& parMesh);

private:
    std::ifstream FInputFile;
};

} // namespace Rendering
} // namespace ECSEngine
