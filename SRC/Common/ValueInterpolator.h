#pragma once

namespace ECSEngine
{
template<typename T>
class ValueInterpolator
{
    static constexpr u32 MaxInterpolationFrames = 5;

public:
    ValueInterpolator() { }

    void Init(const T& parInitialKeyFrame, float parInitialTime)
    {
        FKeyframes.resize(MaxInterpolationFrames);
        FKeyframes[0] = { parInitialTime, parInitialKeyFrame };
        FKeyframesCount = 1;
    }

    void AddNewKeyframe(const T& parKeyFrame, float parTime)
    {
        AlwaysCheckedAssert(FKeyframesCount < MaxInterpolationFrames);
        AlwaysCheckedAssert(FKeyframes[FKeyframesCount - 1].KeyFrameTime <= parTime);

        if (FKeyframes[FKeyframesCount - 1].KeyFrameTime == parTime)
        {
            FKeyframes[FKeyframesCount - 1].Value = parKeyFrame;
        }
        else
        {
            FKeyframes[FKeyframesCount] = { parTime, parKeyFrame };
            FKeyframesCount++;
        }
    }

    void Update(float parCurrentTime)
    {
        AlwaysCheckedAssert(parCurrentTime >= FKeyframes[0].KeyFrameTime);
        if (FKeyframesCount == 1)
        {
            FKeyframes[0].KeyFrameTime = parCurrentTime;
            return;
        }

        // search the current keyframe, in case we forgot one
        u32 currentKeyFrame = 1;
        while (FKeyframes[currentKeyFrame].KeyFrameTime < parCurrentTime && currentKeyFrame < FKeyframesCount)
        {
            currentKeyFrame++;
        }

        // if we reached the end, take the last and put it first
        if (currentKeyFrame == FKeyframesCount)
        {
            FKeyframes[0] = FKeyframes[FKeyframesCount - 1];
            FKeyframes[0].KeyFrameTime = parCurrentTime;
            FKeyframesCount = 1;
            return;
        }

        // if it is not one, drop the keyframes in the middle
        if (currentKeyFrame > 1)
        {
            std::vector<Keyframe> newKeyframes;
            newKeyframes.resize(5);
            u32 j = 0;
            if (FKeyframes[currentKeyFrame].KeyFrameTime != parCurrentTime)
            {
                newKeyframes[0] = FKeyframes[0];
                ++j;
            }
            for (u32 i = currentKeyFrame; i < FKeyframesCount; ++i, ++j)
            {
                newKeyframes[j] = FKeyframes[i];
            }
            FKeyframes = std::move(newKeyframes);
            FKeyframesCount = FKeyframesCount - (currentKeyFrame - 1);
        }

        // lerp the values and store it in the first keyframe
        FKeyframes[0].Value = glm::lerp(
              FKeyframes[0].Value, FKeyframes[1].Value, (parCurrentTime - FKeyframes[0].KeyFrameTime) / (FKeyframes[1].KeyFrameTime - FKeyframes[0].KeyFrameTime));
        FKeyframes[0].KeyFrameTime = parCurrentTime;
    }

    const T& GetCurrentValue() const { return FKeyframes[0].Value; }

private:
    float FCurrentTime = -1.f;
    u32 FKeyframesCount = 1;

    struct Keyframe
    {
        float KeyFrameTime = -1.f;
        T Value;
    };

    std::vector<Keyframe> FKeyframes;
};
} // namespace ECSEngine
