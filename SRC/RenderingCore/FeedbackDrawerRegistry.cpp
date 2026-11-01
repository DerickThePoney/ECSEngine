#include "stdafx.h"

#include "FeedbackDrawerRegistry.h"

#include "Common/IFeedbackDrawer.h"

namespace ECSEngine
{
namespace
{
IFeedbackDrawer* GFeedbackDrawer = nullptr;
}

void SetFeedbackDrawer(IFeedbackDrawer* drawer)
{
    GFeedbackDrawer = drawer;
}

IFeedbackDrawer* GetFeedbackDrawer()
{
    return GFeedbackDrawer;
}
} // namespace ECSEngine
