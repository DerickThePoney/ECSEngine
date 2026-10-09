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
        FMessageRecords.reserve(16384);
    }

    void LogMessage(ELoggingCategory::Type parCategory, const std::string& parMessageToLog);

    const std::vector<MessageRecord>& GetLoggedMessages() const { return FMessageRecords; }

private:
    std::vector<MessageRecord> FMessageRecords;
};

void LogManager::LogMessage(ELoggingCategory::Type parCategory, const std::string& parMessageToLog)
{
    if (FMessageRecords.size() >= FMessageRecords.capacity())
        return;
    FMessageRecords.emplace_back(parCategory, parMessageToLog, std::chrono::system_clock::now());
}

namespace ELoggingCategory
{
const char* GetName(Type parValue)
{
    switch (parValue)
    {
    case ECSEngine::ELoggingCategory::RENDERING:
        return "Rendering";
        break;
    case ECSEngine::ELoggingCategory::GAMEPLAY:
        return "Gameplay";
        break;
    case ECSEngine::ELoggingCategory::UI:
        return "UI";
        break;
    case ECSEngine::ELoggingCategory::INPUT:
        return "Input";
        break;
    case ECSEngine::ELoggingCategory::PHYSICS:
        return "Physics";
        break;
    case ECSEngine::ELoggingCategory::DEBUG_MESSAGE:
        return "Debug";
        break;
    case ECSEngine::ELoggingCategory::WARNING_MESSAGE:
        return "Warning";
        break;
    case ECSEngine::ELoggingCategory::ERROR_MESSAGE:
        return "Error";
        break;
    case ECSEngine::ELoggingCategory::ASSET_COOKING:
        return "AssetCooking";
        break;
    case ECSEngine::ELoggingCategory::SOUND:
        return "Sound";
        break;
    case ECSEngine::ELoggingCategory::LENGTH:
        return "LENGTH";
        break;
    default:
        AssertNotReached();
        return "";
        break;
    }
}

} // namespace ELoggingCategory

namespace Logger
{

void LogMessage(ELoggingCategory::Type parCategory, const std::string& parMessageToLog)
{
    if (!LogManager::HasInstance())
        return;

    LogManager::Instance().LogMessage(parCategory, parMessageToLog);
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
