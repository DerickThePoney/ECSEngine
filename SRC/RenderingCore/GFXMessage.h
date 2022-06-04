#pragma once
#include <mutex>

namespace ECSEngine
{
namespace Rendering
{
class GFXMessage
{
public:
    template<typename T>
    void PushMessage(u32 parKey, const T& parData, const float parTime)
    {
        std::scoped_lock<std::mutex> lock(FMutex);
        auto itFind = FMessages.find(parKey);
        if (itFind != FMessages.end())
        {
            MessageData& message = itFind->second;
            message.Size = sizeof(T);
            memcpy(&message.Data, &parData, sizeof(T));
            return;
        }

        MessageData data;
        data.Size = sizeof(T);
        memcpy(data.Data, &parData, sizeof(T));
        data.Time = parTime;
        FMessages.insert_or_assign(parKey, data);
    }

    template<typename T>
    std::pair<T, float> GetValueIFP(u32 parKey)
    {
        std::scoped_lock<std::mutex> lock(FMutex);
        auto itFind = FMessages.find(parKey);
        if (itFind == FMessages.end())
            return { T(), -1.f };

        return { *((T*)itFind->second.Data), itFind->second.Time };
    }

    bool HasMessage(u32 parKey)
    {
        std::scoped_lock<std::mutex> lock(FMutex);
        auto itFind = FMessages.find(parKey);
        return itFind != FMessages.end();
    }

    bool HasMessages()
    {
        std::scoped_lock<std::mutex> lock(FMutex);
        return FMessages.size() > 0;
    }

    void ClearMessages()
    {
        std::scoped_lock<std::mutex> lock(FMutex);
        FMessages.clear();
    }

private:
    static constexpr u32 MaxDataSize = 512;
    struct MessageData
    {
        MessageData() { memset(&Data, '\0', MaxDataSize); }
        uc8 Data[MaxDataSize];
        u32 Size = 0;
        float Time = 0.f;
    };

    std::unordered_map<u32, MessageData> FMessages;
    std::mutex FMutex;
};
} // namespace Rendering
} // namespace ECSEngine
