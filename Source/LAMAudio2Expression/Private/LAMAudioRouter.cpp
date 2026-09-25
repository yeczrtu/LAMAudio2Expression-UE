#include "LAMAudioRouter.h"
#include "ActiveSound.h"
#include "AudioDevice.h"
#include "Audio.h"
#include "Sound/SoundSubmix.h"
#include "Misc/ScopeLock.h"
#include "UObject/UnrealType.h"

ULAMPlaybackSound* ULAMPlaybackSound::ForSource(USoundWave* Wave)
{
    check(IsInGameThread());
    // Inline concurrency groups use the sound UObject identity. Share policy for active plays
    // of the same source, while keeping routing/gain on their individual audio components.
    static TMap<TWeakObjectPtr<USoundWave>, TWeakObjectPtr<ULAMPlaybackSound>> Sounds;
    for (auto It = Sounds.CreateIterator(); It; ++It)
        if (!It.Key().IsValid() || !It.Value().IsValid()) It.RemoveCurrent();
    if (const auto* Existing = Sounds.Find(Wave)) return Existing->Get();
    auto* Sound = NewObject<ULAMPlaybackSound>();
    Sound->Initialize(Wave);
    Sounds.Add(Wave, Sound);
    return Sound;
}
void ULAMPlaybackSound::Initialize(USoundWave* Wave)
{
    // Copy only the common SoundBase properties. Source-specific PCM/streaming data stays on Wave.
    for (TFieldIterator<FProperty> It(USoundBase::StaticClass()); It; ++It)
        It->CopyCompleteValue_InContainer(this, Wave);
    Source = Wave;
    VirtualizationMode = EVirtualizationMode::PlayWhenSilent;
}
void ULAMPlaybackSound::Parse(FAudioDevice* Device, UPTRINT Hash, FActiveSound& Sound,
    const FSoundParseParameters& Params, TArray<FWaveInstance*>& Waves)
{
    if (auto* Existing = Sound.FindWaveInstance(Hash)) Existing->SoundSubmix = Params.SoundSubmix;
    if (Source) Source->Parse(Device, Hash, Sound, Params, Waves);
    // The explicit per-play pause policy must also override a SoundClass marked as UI.
    if (auto* Wave = Sound.FindWaveInstance(Hash)) Wave->bIsUISound = Sound.bIsUISound;
}

FLAMRouteSnapshot::FLAMRouteSnapshot(const FLAMAudioPlaybackSettings& S)
    : Output(S.OutputSubmix), Inherit(S.bInheritSoundWaveSends), Sends(S.AdditionalSubmixSends)
{
    if (Output) References.Emplace(Output);
    for (const auto& Send : Sends)
        if (Send.Submix) References.Emplace(Send.Submix.Get());
}
void ULAMAudioRouter::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    Hook = NewObject<ULAMPlaybackEndHook>(this);
    Hook->Router = this;
}
void ULAMAudioRouter::Deinitialize()
{
    Bridges.Empty();
    Super::Deinitialize();
}
void ULAMAudioRouter::Add(TSharedPtr<FLAMPlaybackBridge, ESPMode::ThreadSafe> Bridge)
{
    Bridges.Add(Bridge->ComponentId, MoveTemp(Bridge));
}
void ULAMAudioRouter::Remove(uint64 Id) { Bridges.Remove(Id); }
void ULAMAudioRouter::NotifyActiveSoundCreated(FActiveSound& Sound)
{
    if (auto* B = Bridges.Find(Sound.GetAudioComponentID()))
    {
        (*B)->Started = true;
    }
}
void ULAMAudioRouter::NotifyActiveSoundDeleting(const FActiveSound& Sound)
{
    if (auto* B = Bridges.Find(Sound.GetAudioComponentID())) (*B)->Ended = true;
}
void ULAMAudioRouter::SubsystemsAdjustParseParams(const FActiveSound& Sound, FSoundParseParameters& P)
{
    auto* Found = Bridges.Find(Sound.GetAudioComponentID());
    if (!Found) return;
    auto B = *Found;
    TSharedPtr<FLAMRouteSnapshot, ESPMode::ThreadSafe> R;
    {
        FScopeLock Lock(&B->Mutex); R = B->Route;
        if (B->RenderReady)
        if (auto* Wave = const_cast<FActiveSound&>(Sound).FindWaveInstance(0))
            if (auto* Source = GetAudioDeviceHandle()->GetSoundSource(Wave))
            {
                if (Source->IsInitialized())
                {
                    B->ClockPercent = Source->GetPlaybackPercent();
                    B->ClockUpdated = FPlatformTime::Seconds();
                }
            }
    }
    if (!R) return;
    P.VolumeMultiplier *= B->Gain.Load();
    if (R->Output) { P.SoundSubmix = R->Output; P.bEnableBaseSubmix = true; }
    if (!R->Inherit) P.SoundSubmixSends.Reset();
    for (const auto& S : R->Sends)
    {
        if (!S.Submix) continue;
        P.SoundSubmixSends.RemoveAll([&](const FSoundSubmixSendInfo& V) { return V.SoundSubmix == S.Submix; });
        FSoundSubmixSendInfo Send;
        Send.SoundSubmix = S.Submix;
        Send.SendLevel = FMath::Clamp(S.Level, 0.f, 1.f);
        P.SoundSubmixSends.Add(Send);
    }
    if (!R->Sends.IsEmpty()) P.bEnableSubmixSends = true;
    P.NotifyBufferFinishedHooks.AddNotify(Hook, 0);
}
void ULAMAudioRouter::WaveEnded(FWaveInstance* Wave)
{
    if (!Wave || !Wave->ActiveSound) return;
    auto* B = Bridges.Find(Wave->ActiveSound->GetAudioComponentID());
    if (!B) return;
    // Forced Stop removes the source map entry before notifying. A stopping voice can also
    // notify on its short hardware fade, which is not source EOF.
    auto* Source = GetAudioDeviceHandle()->GetSoundSource(Wave);
    if (Source && !Wave->IsStopping() && !Source->IsStopping() && Source->GetPlaybackPercent() >= 1.f)
        (*B)->NaturalEnd = true;
}
bool ULAMPlaybackEndHook::NotifyWaveInstanceFinished(FWaveInstance* Wave)
{
    if (Router) Router->WaveEnded(Wave);
    return false;
}
