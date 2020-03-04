#include "stdafx.h"

#include "MeshFileStreaming.h"

namespace ECSEngine
{
namespace Rendering
{
#ifdef WITH_ASSETS_GENERATION
MeshFileWriter::MeshFileWriter(const std::string& parFilename)
{
}

MeshFileWriter::~MeshFileWriter()
{
}

void MeshFileWriter::operator<<(const void* parMeshData)
{
}

#endif

} // namespace Rendering
} // namespace ECSEngine