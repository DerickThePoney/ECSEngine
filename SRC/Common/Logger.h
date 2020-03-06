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
}

struct MessageRecord
{
    ELoggingCategory::Type Category;
    std::string Message;
    std::chrono::high_resolution_clock::time_point LogTime;
};

namespace Logger
{
void LogMessage(ELoggingCategory::Type parCategory, std::string&& parMessageToLog);
void InitLogger();
void ShutdownLogger();
const std::vector<MessageRecord>& GetLoggedMessages();
} // namespace Logger

#ifdef PERFORM_SECURITY_CHECKS
#define LOG_RENDERING(MSG) LogMessage(ELoggingCategory::RENDERING, std::move(MSG))
#define LOG_GAMEPLAY(MSG) LogMessage(ELoggingCategory::GAMEPLAY, std::move(MSG))
#define LOG_UI(MSG) LogMessage(ELoggingCategory::UI, std::move(MSG))
#define LOG_DEBUG(MSG) LogMessage(ELoggingCategory::DEBUG, std::move(MSG))
#define LOG_ERROR(MSG) LogMessage(ELoggingCategory::ERROR, std::move(MSG))
#else
#define LOG_RENDERING(MSG)
#define LOG_GAMEPLAY(MSG)
#define LOG_UI(MSG)
#define LOG_DEBUG(MSG)
#define LOG_ERROR(MSG)
#endif
} // namespace ECSEngine