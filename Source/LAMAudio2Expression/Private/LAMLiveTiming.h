#pragma once
#include "CoreMinimal.h"
#include "LAMTypes.h"

namespace LAM
{
inline int32 LiveHop(float Milliseconds)
{
    return FMath::IsFinite(Milliseconds) ? FMath::Clamp(FMath::RoundToInt(FMath::Clamp(Milliseconds, 0.f, 1000.f) * FPS / 1000.f), 1, 30) : 10;
}
inline int64 NextLiveFrame(int64 Samples, int64 LastFrame, int32 Hop)
{
    const int64 Available = Samples * FPS / Rate;
    return Available > LastFrame ? LastFrame + ((Available - LastFrame) / Hop) * Hop : LastFrame;
}
inline float LiveP95(const TArray<float>& Values)
{
    if (Values.IsEmpty()) return 0;
    auto Sorted = Values; Sorted.Sort();
    return Sorted[FMath::Clamp(FMath::CeilToInt(Sorted.Num() * .95f) - 1, 0, Sorted.Num() - 1)];
}
inline float LiveDelay(float MinimumSeconds, float PreviousMs, float IntervalMs, float LatencyMs)
{
    const float Minimum = FMath::IsFinite(MinimumSeconds) ? FMath::Clamp(MinimumSeconds, .4f, 2.f) : .75f;
    return FMath::Min(2000.f, FMath::Max(PreviousMs, FMath::Max(Minimum * 1000, IntervalMs + LatencyMs + 1000.f / FPS)));
}
}
