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

        std::scoped_lock<std::mutex> lock(FMutex);

        foreachitem(message, FMessagesQueues[Id]) { parFunctor(*message->UserDataAs<UserData>()); }
        FMessagesQueues[Id].clear();
    }

private:
    std::vector<std::vector<std::unique_ptr<GenericMessage>>> FMessagesQueues;
    std::mutex FMutex;
};
} // namespace ECSEngine