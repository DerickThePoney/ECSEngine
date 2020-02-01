#pragma once

namespace ECSEngine
{
namespace Rendering
{
class Camera
{

private:
    glm::vec3 FPostion;
    glm::quat FOrientation;

    float FFov;
};
} // namespace Rendering
} // namespace ECSEngine