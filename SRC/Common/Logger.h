#pragma once

namespace ECSEngine
{
namespace ELoggingCategory
{
enum Type
{
    RENDERING,
    GAMEPLAY,
    UI,
    INPUT,
    PHYSICS,
    DEBUG_MESSAGE,
    WARNING_MESSAGE,
    ERROR_MESSAGE,
    ASSET_COOKING,
    SOUND,
    LENGTH
};

const char* GetName(Type parValue);
} // namespace ELoggingCategory

struct MessageRecord
{
    MessageRecord(const ELoggingCategory::Type parType, const std::string& parMessage, const std::chrono::system_clock::time_point parLogTime)
        : Category(parType)
        , Message(parMessage)
        , LogTime(parLogTime)
    {
    }

    ELoggingCategory::Type Category;
    std::string Message;
    std::chrono::system_clock::time_point LogTime;
};

namespace Logger
{
void LogMessage(ELoggingCategory::Type parCategory, const std::string& parMessageToLog);
void InitLogger();
void ShutdownLogger();
const std::vector<MessageRecord>& GetLoggedMessages();
} // namespace Logger

#ifdef PERFORM_SECURITY_CHECKS
#define LOG_RENDERING(MSG) Logger::LogMessage(ELoggingCategory::RENDERING, MSG)
#define LOG_GAMEPLAY(MSG) Logger::LogMessage(ELoggingCategory::GAMEPLAY, MSG)
#define LOG_UI(MSG) Logger::LogMessage(ELoggingCategory::UI, MSG)
#define LOG_INPUT(MSG) Logger::LogMessage(ELoggingCategory::INPUT, MSG)
#define LOG_PHYSICS(MSG) Logger::LogMessage(ELoggingCategory::PHYSICS, MSG)
#define LOG_DEBUG(MSG) Logger::LogMessage(ELoggingCategory::DEBUG_MESSAGE, MSG)
#define LOG_WARNING(MSG) Logger::LogMessage(ELoggingCategory::WARNING_MESSAGE, MSG)
#define LOG_ERROR(MSG) Logger::LogMessage(ELoggingCategory::ERROR_MESSAGE, MSG)
#define LOG_COOKING(MSG) Logger::LogMessage(ELoggingCategory::ASSET_COOKING, MSG)
#define LOG_SOUND(MSG) Logger::LogMessage(ELoggingCategory::SOUND, MSG)
#else
#define LOG_RENDERING(MSG)
#define LOG_GAMEPLAY(MSG)
#define LOG_UI(MSG)
#define LOG_DEBUG(MSG)
#define LOG_WARNING(MSG)
#define LOG_ERROR(MSG)
#define LOG_COOKING(MSG)
#define LOG_SOUND(MSG)
#endif
} // namespace ECSEngine
