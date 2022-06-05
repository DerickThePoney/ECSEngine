#pragma once

namespace ECSEngine
{
class ModuleTemplate;
namespace EntityTemplateManagerMethods
{
bool RegisterTemplateFactory(const u32 parId, ModuleTemplate* (*parFactory)());
ModuleTemplate* CreateModuleTemplate(const u32 parId);
const std::map<u32, std::string>& GetModuleList();
void Cleanup();
} // namespace EntityTemplateManagerMethods
} // namespace ECSEngine