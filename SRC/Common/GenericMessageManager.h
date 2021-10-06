#pragma once
#include "GenericMessage.h"
#include "Singleton.h"

namespace ECSEngine
{
class GenericMessageManager : public Singleton<GenericMessageManager>
{
public:
    template<u32 Id, typename UserData>
    void PushMessage(UserData* parMessageDataStealOwnership)
    {
        std::scoped_lock<std::mutex> lock(FMutex);

        if (FMessagesQueues.size() <= Id)
            FMessagesQueues.resize(Id + 1);

        FMessagesQueues[Id].push_back(std::unique_ptr<GenericMessage>(new GenericMessageImplem<UserData>(Id, (void*)parMessageDataStealOwnership)));
    }

    template<u32 Id, typename UserData, typename Functor>
    void ProcessMessages(Functor& parFunctor)
    {
        if (FMessagesQueues.size() <= Id)
            return;
        if (FMessagesQueues[Id].empty())
            return;

        std::vector<std::unique_ptr<GenericMessage>> messages;
        {
            std::scoped_lock<std::mutex> lock(FMutex);
            messages.reserve(FMessagesQueues[Id].size());
            foreachitem(message, FMessagesQueues[Id]) { messages.push_back(std::move(message)); }
            FMessagesQueues[Id].clear();
        }
        foreachitem(message, messages) { parFunctor(*message->UserDataAs<UserData>()); }
    }

private:
    std::vector<std::vector<std::unique_ptr<GenericMessage>>> FMessagesQueues;
    std::mutex FMutex;
};
} // namespace ECSEngine
