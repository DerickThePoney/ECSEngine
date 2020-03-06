#include "stdafx.h"

#include "Logger.h"

#include "Singleton.h"

namespace ECSEngine
{

class LogManager final : public Singleton<LogManager>
{
public:
    LogManager()
        : Singleton()
    {
    }

    void LogMessage(ELoggingCategory::Type parCategory, std::string&& parMessageToLog);

    const std::vector<MessageRecord>& GetLoggedMessages() const { return FMessageRecords; }

private:
    std::vector<MessageRecord> FMessageRecords;
};

void LogManager::LogMessage(ELoggingCategory::Type parCategory, std::string&& parMessageToLog)
{
    FMessageRecords.push_back({ parCategory, std::move(parMessageToLog), std::chrono::high_resolution_clock::now() });
}

namespace Logger
{

void LogMessage(ELoggingCategory::Type parCategory, std::string&& parMessageToLog)
{
    if (!LogManager::HasInstance())
        return;

    LogManager::Instance().LogMessage(parCategory, std::move(parMessageToLog));
}

void InitLogger()
{
    LogManager::CreateIFP();
    AssertRelease(LogManager::HasInstance());
}

void ShutdownLogger()
{
    LogManager::Destroy();
    AssertRelease(!LogManager::HasInstance());
}

const std::vector<ECSEngine::MessageRecord>& GetLoggedMessages()
{
    AssertRelease(LogManager::HasInstance());
    return LogManager::Instance().GetLoggedMessages();
}

} // namespace Logger
} // namespace ECSEngine
