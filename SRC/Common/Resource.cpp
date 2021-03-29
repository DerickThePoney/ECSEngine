#include "stdafx.h"

#include "Resource.h"

#include <algorithm>

namespace ECSEngine
{

Resource::Resource(const std::string& parName)
    : FName(parName)
{
    std::transform(FName.begin(), FName.end(), FName.begin(), [](char c) { return std::tolower(c); });
    std::replace(FName.begin(), FName.end(), '/', '\\');
}

} // namespace ECSEngine
