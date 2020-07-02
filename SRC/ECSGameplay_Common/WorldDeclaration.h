#pragma once
#include "ECSCore/ModuleId.h"
namespace ECSEngine
{

void CreateWorlds();

namespace ModuleTemplates
{
void InitModuleTemplateFactories();
void DestroyModyleTemplateFactories();
} // namespace ModuleTemplates

} // namespace ECSEngine
