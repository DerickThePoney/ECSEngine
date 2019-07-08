#include "stdafx.h"

#include "Resource.h"

#include <algorithm>

namespace ECSEngine
{

Resource::Resource(const std::string& parName)
    : FName(parName)
{
    std::transform(FName.begin(), FName.end(), FName.begin(), std::tolower);
}

} // namespace ECSEngine
