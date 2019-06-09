#pragma once

namespace ECSEngine
{
class AssertImplementation
{
public:
    AssertImplementation(const bool shouldLetGo, const std::string& msg);
    ~AssertImplementation() = default;

private:
    void Assert(const std::string& msg);
};

#ifndef ABSOLUTELY_NOT_ASSERT
#define AlwaysCheckedAssert(CND)                                                                                                                                                   \
    {                                                                                                                                                                              \
        ECSEngine::AssertImplementation(CND, #CND);                                                                                                                                \
    }
#define AssertRelease(CND) AlwaysCheckedAssert(CND)

#define AssertNotReached() AssertRelease(false)

#else

#define AlwaysCheckedAssert(CND) DONOTHING
#define AssertRelease(CND) DONOTHING
#define AssertNotReached() DONOTHING
#endif

} // namespace ECSEngine