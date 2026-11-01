#pragma once

namespace ECSEngine
{
class IFeedbackDrawer;

void SetFeedbackDrawer(IFeedbackDrawer* drawer);
IFeedbackDrawer* GetFeedbackDrawer();
} // namespace ECSEngine
