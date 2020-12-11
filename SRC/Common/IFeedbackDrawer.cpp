#include "stdafx.h"

#include "IFeedbackDrawer.h"

namespace ECSEngine
{

void IFeedbackDrawer::DrawFeedback(Rendering::DrawCommandBuffer* parCommandBuffer)
{
    VirtualDrawFeedback(parCommandBuffer);
}

} // namespace ECSEngine
