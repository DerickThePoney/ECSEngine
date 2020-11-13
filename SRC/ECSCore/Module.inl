#include "WorldManager.h"

namespace ECSEngine
{
template<typename T>
const T* Module::Template() const
{
    AssertRelease(FTemplate != nullptr);

    return static_cast<const T*>(FTemplate);
}
} // namespace ECSEngine
