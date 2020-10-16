#pragma once

namespace ECSEngine
{
class AssertImplementation
{
public:
    AssertImplementation(const bool shouldLetGo, const char* msg, const char* additionalMessage);
    ~AssertImplementation() = default;

private:
    void Assert(const char* msg, const char* additionalMessage);
};

#ifndef ABSOLUTELY_NOT_ASSERT
#define AlwaysCheckedAssertMsg(CND, MSG)                                                                                                                                           \
    {                                                                                                                                                                              \
        ECSEngine::AssertImplementation(CND, #CND, MSG);                                                                                                                           \
    }
#define AlwaysCheckedAssert(CND)                                                                                                                                                   \
    {                                                                                                                                                                              \
        ECSEngine::AssertImplementation(CND, #CND, nullptr);                                                                                                                       \
    }
#define AssertRelease(CND) AlwaysCheckedAssert(CND)
#define AssertReleaseMsg(CND, MSG) AlwaysCheckedAssertMsg(CND, MSG)

#define AssertNotReached() AssertRelease(false)
#define AssertNotReachedMsg(MSG) AssertReleaseMsg(false, MSG)

#define COMMA ,
#define OnlyWithAssertions(CMD) CMD

#else

#define AlwaysCheckedAssert(CND) DONOTHING
#define AssertRelease(CND) DONOTHING
#define AssertNotReached() DONOTHING

#define AlwaysCheckedAssertMsg(CND, MSG) DONOTHING
#define AssertReleaseMsg(CND, MSG) DONOTHING
#define AssertNotReachedMsg(MSG) DONOTHING
#define COMMA
#define OnlyWithAssertions(CMD)
#endif

} // namespace ECSEngine