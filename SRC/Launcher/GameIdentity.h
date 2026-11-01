#pragma once

namespace ECSEngine
{
class BaseApplicationLayer;

void InstallDefaultApplication(BaseApplicationLayer& app);
const char* GetGameAssetsDirectory();
} // namespace ECSEngine
