#include "stdafx.h"

#include "RmlSystemInterface.h"

#include "Common/TimeManager.h"

namespace ECSEngine
{
namespace UI
{

double RmlSystemInterface::GetElapsedTime()
{
    return TimeManager::FrameDeltaTime();
}

} // namespace UI
} // namespace ECSEngine