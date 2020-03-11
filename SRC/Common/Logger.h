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
    DEBUG_MESSAGE,
    WARNING_MESSAGE,
    ERROR_MESSAGE,
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
void LogMessage(ELoggingCategory::Type parCategory, const std::string parMessageToLog);
void InitLogger();
void ShutdownLogger();
const std::vector<MessageRecord>& GetLoggedMessages();
} // namespace Logger

#ifdef PERFORM_SECURITY_CHECKS
#define LOG_RENDERING(MSG) Logger::LogMessage(ELoggingCategory::RENDERING, MSG)
#define LOG_GAMEPLAY(MSG) Logger::LogMessage(ELoggingCategory::GAMEPLAY, MSG)
#define LOG_UI(MSG) Logger::LogMessage(ELoggingCategory::UI, MSG)
#define LOG_DEBUG(MSG) Logger::LogMessage(ELoggingCategory::DEBUG_MESSAGE, MSG)
#define LOG_WARNING(MSG) Logger::LogMessage(ELoggingCategory::WARNING_MESSAGE, MSG)
#define LOG_ERROR(MSG) Logger::LogMessage(ELoggingCategory::ERROR_MESSAGE, MSG)
#else
#define LOG_RENDERING(MSG)
#define LOG_GAMEPLAY(MSG)
#define LOG_UI(MSG)
#define LOG_DEBUG(MSG)
#define LOG_WARNING(MSG)
#define LOG_ERROR(MSG)
#endif
} // namespace ECSEngine