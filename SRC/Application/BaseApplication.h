#pragma once

namespace ECSEngine
{
class ILoader;
class IGameplayUpdater;
class BaseApplicationLayer
{
public:
    BaseApplicationLayer();
    virtual ~BaseApplicationLayer();

    void Initialise();
    void Shutdown();

    void RunMainLoop();

    void SetGameplayUpdater_StealOwnership(IGameplayUpdater* parGameplayUpdater);

    template<class Loader, class... U>
    void AddNewLoader(U&&... u)
    {
        FLoaders.push_back(std::unique_ptr<ILoader>(new Loader(std::forward<U>(u)...)));
    }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(FLoaders, FGameplayUpdater);
    }

private:
    std::vector<std::unique_ptr<ILoader>> FLoaders;
    std::unique_ptr<IGameplayUpdater> FGameplayUpdater;
};
} // namespace ECSEngine
