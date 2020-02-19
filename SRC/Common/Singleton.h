#pragma once

namespace ECSEngine
{
template<typename T>
class Singleton
{
public:
    virtual ~Singleton() {}

public:
    static void CreateIFP()
    {
        if (FInstance == nullptr)
        {
            FInstance = new T();
        }
    }

    static void Destroy()
    {
        if (FInstance != nullptr)
        {
            delete FInstance;
            FInstance = nullptr;
        }
    }

    static bool HasInstance() { return FInstance != nullptr; }
    static T& Instance()
    {
        AssertRelease(FInstance != nullptr);
        return *FInstance;
    }

private:
    inline static T* FInstance = nullptr;
};

} // namespace ECSEngine