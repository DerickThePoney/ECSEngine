#include "stdafx.h"

#include "WidgetScaler.h"

#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{
namespace UI
{

glm::vec2 WidgetScaler::GetScale() const
{
    return Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
}

} // namespace UI
} // namespace ECSEngine
