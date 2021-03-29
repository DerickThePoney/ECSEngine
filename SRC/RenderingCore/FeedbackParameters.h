#pragma once

namespace ECSEngine
{
namespace Rendering
{
struct CircleFeedbackParameters
{
    float Range = 20.f;
    float Thickness = 1.f;
    glm::vec4 Color = glm::vec4(0.f);
};

struct CircularGridChunkFeedbackParameters
{
    float InnerCircleRadius = 1.0f;
    float OuterCircleRadius = 2.0f;
    float Thickness = 0.1f;
    float ArcAngle = 2.f;
    u32 Color = 0;
};
} // namespace Rendering
} // namespace ECSEngine
