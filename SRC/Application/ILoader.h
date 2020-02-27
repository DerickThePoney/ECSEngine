#pragma once

namespace ECSEngine
{
class ILoader
{
public:
    ILoader();
    virtual ~ILoader();

    bool Initialise();
    void Shutdown();

    template<typename Archive>
    void save(Archive& ar) const
    {
#ifdef PERFORM_SECURITY_CHECKS
        ar(PROPERTY(LoaderName));
#else
        std::string dummy("dummy");
        ar(NAMEDPROPERTY("LoaderName", dummy));
#endif
    }

    template<typename Archive>
    void load(Archive& ar)
    {
#ifdef PERFORM_SECURITY_CHECKS
        ar(FLoaderName);
#else
        std::string dummy;
        ar(dummy);
#endif
    }

protected:
    virtual bool VirtualInitialise();
    virtual void VirtualShutdown();

private:
#ifdef PERFORM_SECURITY_CHECKS
    std::string FLoaderName;

    bool FVirtualInitialisedCalled = false;
    bool FVirtualShutdownCalled = false;
#endif

    bool FHasBeenInitialised = false;
};
} // namespace ECSEngine
