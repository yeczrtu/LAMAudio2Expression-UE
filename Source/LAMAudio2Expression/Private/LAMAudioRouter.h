#pragma once
#include "CoreMinimal.h"
#include "Subsystems/AudioEngineSubsystem.h"
#include "ActiveSoundUpdateInterface.h"
#include "Sound/SoundNode.h"
#include "LAMPlaybackTypes.h"
#include "UObject/StrongObjectPtr.h"
#include "LAMAudioRouter.generated.h"

// Created on the game thread. The audio thread only reads immutable routing snapshots.
struct FLAMRouteSnapshot
{
    explicit FLAMRouteSnapshot(const FLAMAudioPlaybackSettings& Settings);
    USoundSubmixBase* Output = nullptr;
    bool Inherit = true;
    TArray<FLAMSubmixSend> Sends;
    TArray<TStrongObjectPtr<UObject>> References;
};
struct FLAMPlaybackBridge
{
    FCriticalSection Mutex;
    TSharedPtr<FLAMRouteSnapshot, ESPMode::ThreadSafe> Route;
    float ClockPercent = -1;
    double ClockUpdated = 0;
    TAtomic<float> Gain{1};
    TAtomic<bool> Started{false}, Ended{false}, NaturalEnd{false}, RenderReady{false};
    uint64 ComponentId = 0;
};

class FLAMSourceReadyListener : public ISourceBufferListener
{
public:
    explicit FLAMSourceReadyListener(TSharedPtr<FLAMPlaybackBridge, ESPMode::ThreadSafe> InBridge) : Bridge(MoveTemp(InBridge)) {}
    void OnNewBuffer(const FOnNewBufferParams&) override { Bridge->RenderReady = true; }
    void OnSourceReleased(int32) override { Bridge->RenderReady = false; }
private:
    TSharedPtr<FLAMPlaybackBridge, ESPMode::ThreadSafe> Bridge;
};

// A lightweight per-source sound holds policy, never PCM or a duplicated cooked bulk payload.
// UE 5.8's FActiveSound virtualization setter is not exported from Engine in modular builds.
UCLASS()
class ULAMPlaybackSound : public USoundBase
{
    GENERATED_BODY()
public:
    static ULAMPlaybackSound* ForSource(USoundWave* Wave);
    void Initialize(USoundWave* Wave);
    virtual bool IsPlayable() const override { return Source && Source->IsPlayable(); }
    virtual float GetDuration() const override { return Source ? Source->GetDuration() : 0; }
    virtual void InitResources() override { if (Source) Source->InitResources(); }
    virtual void Parse(FAudioDevice* Device, UPTRINT Hash, FActiveSound& ActiveSound,
        const FSoundParseParameters& Params, TArray<FWaveInstance*>& Waves) override;
    UPROPERTY() TObjectPtr<USoundWave> Source;
};

UCLASS()
class ULAMPlaybackEndHook : public USoundNode
{
    GENERATED_BODY()
public:
    virtual bool NotifyWaveInstanceFinished(FWaveInstance* Wave) override;
    UPROPERTY() TObjectPtr<class ULAMAudioRouter> Router;
};

UCLASS()
class ULAMAudioRouter : public UAudioEngineSubsystem, public IActiveSoundUpdateInterface
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void NotifyActiveSoundCreated(FActiveSound& Sound) override;
    virtual void NotifyActiveSoundDeleting(const FActiveSound& Sound) override;
    virtual void SubsystemsAdjustParseParams(const FActiveSound& Sound, FSoundParseParameters& Params) override;
    void Add(TSharedPtr<FLAMPlaybackBridge, ESPMode::ThreadSafe> Bridge);
    void Remove(uint64 ComponentId);
    void WaveEnded(FWaveInstance* Wave);
private:
    UPROPERTY() TObjectPtr<ULAMPlaybackEndHook> Hook;
    TMap<uint64, TSharedPtr<FLAMPlaybackBridge, ESPMode::ThreadSafe>> Bridges;
};
