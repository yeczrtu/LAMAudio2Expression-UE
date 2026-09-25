#include "LAMAudio2ExpressionComponent.h"
#include "LAMAnalyzeAsync.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundSubmix.h"
#include "LAMLive.h"
#include "LAMAudioRouter.h"
#include "AudioDevice.h"
#include "AudioThread.h"
#include "Engine/World.h"

ULAMAudio2ExpressionComponent::ULAMAudio2ExpressionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    PrimaryComponentTick.bTickEvenWhenPaused = true;
}
ULAMAudio2ExpressionComponent::~ULAMAudio2ExpressionComponent() = default;
void ULAMAudio2ExpressionComponent::SetAnalysis(ULAMAnalyzeAsync* A) { CancelAnalysis(); Analysis = A; }
void ULAMAudio2ExpressionComponent::CancelAnalysis() { if (Analysis) Analysis->Cancel(); Analysis = nullptr; }

FLAMPlaybackInfo ULAMAudio2ExpressionComponent::GetPlaybackInfo() const
{
    auto Info = PlaybackInfo;
    Info.Position = bPlaying ? Frame.TimeSeconds : PlaybackInfo.Position;
    Info.Progress = Info.Duration > 0 ? FMath::Clamp(Info.Position / Info.Duration, 0.f, 1.f) : 0;
    return Info;
}
void ULAMAudio2ExpressionComponent::ReportPlaybackError(ULAMExpressionClip* Clip, int64 Id, FName Code, const FString& Message)
{
    if (bEndingPlay) return;
    FLAMPlaybackError E; E.PlaybackId = Id; E.Clip = Clip; E.Code = Code; E.Message = Message;
    OnPlaybackFailed.Broadcast(E);
}
void ULAMAudio2ExpressionComponent::SetPlaybackState(ELAMPlaybackState State)
{
    if (PlaybackInfo.State == State) return;
    PlaybackInfo.State = State;
    const auto Info = GetPlaybackInfo();
    if (!bEndingPlay) OnPlaybackStateChanged.Broadcast(Info);
}
void ULAMAudio2ExpressionComponent::UpdateRouting()
{
    if (!PlaybackBridge) return;
    auto R = MakeShared<FLAMRouteSnapshot, ESPMode::ThreadSafe>(ActivePlaybackSettings);
    { FScopeLock Lock(&PlaybackBridge->Mutex); PlaybackBridge->Route = MoveTemp(R); }
    PlaybackBridge->Gain = ActivePlaybackSettings.bMuted ? 0 : ActivePlaybackSettings.Volume * FadeGain;
}
void ULAMAudio2ExpressionComponent::ReleasePlaybackAudio()
{
    if (Audio)
    {
        Audio->OnAudioPlaybackPercentNative.RemoveAll(this);
        Audio->OnAudioFinishedNative.RemoveAll(this);
        auto Device = GetWorld()->GetAudioDevice();
        const uint64 Id = Audio->GetAudioComponentID();
        Audio->Stop();
        if (Device.IsValid()) FAudioThread::RunCommandOnAudioThread([Device, Id]() {
            if (auto* R = Device->GetSubsystem<ULAMAudioRouter>()) R->Remove(Id);
        });
        Audio->DestroyComponent();
        Audio = nullptr;
    }
    PlaybackBridge.Reset();
}
void ULAMAudio2ExpressionComponent::EndPlayback(ELAMPlaybackEndReason Reason, FName Code, const FString& Message)
{
    if (!bPlaying) return;
    auto Info = GetPlaybackInfo();
    TStrongObjectPtr<ULAMExpressionClip> KeepClip(Info.Clip.Get());
    Info.State = Reason == ELAMPlaybackEndReason::Failed ? ELAMPlaybackState::Failed : ELAMPlaybackState::Stopped;
    if (Reason == ELAMPlaybackEndReason::Completed) { Info.Position = Info.Duration; Info.Progress = 1; }
    bPlaying = bPaused = false;
    bFadeStops = false;
    PlaybackInfo = Info;
    ReleasePlaybackAudio();
    // Finish all internal mutation before broadcasting. A listener may immediately start another clip.
    if (bEndingPlay) return;
    OnPlaybackStateChanged.Broadcast(Info);
    if (bEndingPlay) return;
    OnPlaybackEnded.Broadcast(Info, Reason);
    if (bEndingPlay) return;
    if (Reason == ELAMPlaybackEndReason::Completed) OnPlaybackFinished.Broadcast(Info);
    if (Reason == ELAMPlaybackEndReason::Failed) ReportPlaybackError(Info.Clip, Info.PlaybackId, Code, Message);
}
bool ULAMAudio2ExpressionComponent::PlayExpressionClip(ULAMExpressionClip* Clip, float Start)
{
    return PlayExpressionClipWithSettings(Clip, PlaybackSettings, Start);
}
bool ULAMAudio2ExpressionComponent::PlayExpressionClipWithSettings(ULAMExpressionClip* Clip, const FLAMAudioPlaybackSettings& Settings, float Start)
{
    if (bEndingPlay) return false;
    const int64 RequestId = ++NextPlaybackId;
    const auto RequestedSettings = Settings;
    if (!IsValid(Clip) || !IsValid(Clip->SoundWave) || Clip->Curves.IsEmpty() || Clip->Curves.Num() % 52 || !FMath::IsFinite(Clip->Duration) || Clip->Duration <= 0 ||
        !GetWorld() || !GetOwner() || !FMath::IsFinite(Start) || !FMath::IsFinite(Settings.Volume) ||
        Settings.Volume < 0 || !FMath::IsFinite(Settings.FadeInDuration) || Settings.FadeInDuration < 0 || Clip->SoundWave->IsLooping())
    {
        ReportPlaybackError(Clip, RequestId, TEXT("InvalidPlayback"), TEXT("A non-looping expression clip, finite start time, volume and fade duration are required."));
        return false;
    }
    for (const auto& Send : Settings.AdditionalSubmixSends)
        if (!IsValid(Send.Submix) || !FMath::IsFinite(Send.Level) || Send.Level < 0 || Send.Level > 1)
        { ReportPlaybackError(Clip, RequestId, TEXT("InvalidSubmixSend"), TEXT("Submix sends require a target and a level between zero and one.")); return false; }
    if (Settings.PlaybackMode == ELAMPlaybackMode::ThreeDimensional)
    {
        auto* Parent = Settings.Attachment ? Settings.Attachment.Get() : GetOwner()->GetRootComponent();
        if (!IsValid(Parent) || Parent->GetWorld() != GetWorld() || !Parent->IsRegistered() ||
            (!Settings.Socket.IsNone() && !Parent->DoesSocketExist(Settings.Socket)) || !Settings.Transform.IsValid())
        { ReportPlaybackError(Clip, RequestId, TEXT("InvalidAttachment"), TEXT("3D playback requires a registered attachment and a valid socket/transform in this world.")); return false; }
    }
    const uint64 Mutation = ++PlaybackMutation;
    EndPlayback(ELAMPlaybackEndReason::Replaced);
    if (Mutation != PlaybackMutation || bEndingPlay) return false;
    StopMicrophone();
    if (Mutation != PlaybackMutation || bEndingPlay) return false;
    CurrentClip = Clip;
    ActivePlaybackSettings = RequestedSettings;
    PlaybackInfo = FLAMPlaybackInfo();
    PlaybackInfo.PlaybackId = RequestId; PlaybackInfo.Clip = Clip; PlaybackInfo.Duration = Clip->Duration;
    bPlaying = true; bPaused = false; bStartedNotified = false;
    FadeElapsed = 0; FadeDuration = RequestedSettings.FadeInDuration; FadeFrom = FadeDuration > 0 ? 0 : 1;
    FadeGain = FadeFrom; FadeTo = 1; bFadeStops = false;
    if (!CreatePlaybackAudio(Start))
    { EndPlayback(ELAMPlaybackEndReason::Failed, TEXT("AudioDeviceUnavailable"), TEXT("The audio device or LAM routing subsystem is unavailable.")); return false; }
    SetPlaybackState(ELAMPlaybackState::Starting);
    return true;
}
bool ULAMAudio2ExpressionComponent::CreatePlaybackAudio(float Start)
{
    ReleasePlaybackAudio();
    Audio = NewObject<UAudioComponent>(GetOwner());
    Audio->bAutoActivate = false; Audio->bAutoDestroy = false;
    auto* PlaybackSound = ULAMPlaybackSound::ForSource(CurrentClip->SoundWave);
    Audio->SetSound(PlaybackSound);
    Audio->SetPitchMultiplier(1);
    Audio->SetUISound(ActivePlaybackSettings.bPlayWhenGamePaused);
    Audio->SoundClassOverride = ActivePlaybackSettings.SoundClassOverride;
    Audio->ConcurrencySet = ActivePlaybackSettings.ConcurrencySettings;
    Audio->bAllowSpatialization = ActivePlaybackSettings.PlaybackMode == ELAMPlaybackMode::ThreeDimensional;
    if (Audio->bAllowSpatialization)
    {
        auto* Parent = ActivePlaybackSettings.Attachment ? ActivePlaybackSettings.Attachment.Get() : GetOwner()->GetRootComponent();
        if (!IsValid(Parent) || !Parent->IsRegistered() || Parent->GetWorld() != GetWorld() ||
            (!ActivePlaybackSettings.Socket.IsNone() && !Parent->DoesSocketExist(ActivePlaybackSettings.Socket)))
        { ReleasePlaybackAudio(); return false; }
        Audio->SetupAttachment(Parent, ActivePlaybackSettings.Socket);
        Audio->SetRelativeTransform(ActivePlaybackSettings.Transform);
        Audio->SetAttenuationSettings(ActivePlaybackSettings.AttenuationSettings);
    }
    else
    {
        FSoundAttenuationSettings NoAttenuation;
        NoAttenuation.bAttenuate = false; NoAttenuation.bSpatialize = false;
        Audio->SetAttenuationOverrides(NoAttenuation);
    }
    Audio->RegisterComponent();
    auto Device = GetWorld()->GetAudioDevice();
    if (!Device.IsValid()) { ReleasePlaybackAudio(); return false; }
    PlaybackBridge = MakeShared<FLAMPlaybackBridge, ESPMode::ThreadSafe>();
    PlaybackBridge->ComponentId = Audio->GetAudioComponentID();
    Audio->SetSourceBufferListener(MakeShared<FLAMSourceReadyListener, ESPMode::ThreadSafe>(PlaybackBridge), false);
    UpdateRouting();
    auto Bridge = PlaybackBridge;
    FAudioThread::RunCommandOnAudioThread([Device, Bridge]() {
        if (auto* R = Device->GetSubsystem<ULAMAudioRouter>()) R->Add(Bridge);
        else Bridge->Ended = true;
    });
    Audio->OnAudioPlaybackPercentNative.AddUObject(this, &ULAMAudio2ExpressionComponent::PlaybackPercent);
    Audio->OnAudioFinishedNative.AddUObject(this, &ULAMAudio2ExpressionComponent::PlaybackFinished);
    AudioPosition = FMath::Clamp(Start, 0.f, FMath::Max(0.f, CurrentClip->Duration - KINDA_SMALL_NUMBER));
    bHaveClock = false; bEngineEnded = false; LastUpdate = FPlatformTime::Seconds();
    Frame = CurrentClip->Sample(AudioPosition); Frame.Weight = 0;
    PlaybackInfo.Position = AudioPosition;
    Audio->Play(AudioPosition);
    if (bPaused) { Audio->SetPaused(true); bHaveClock = true; Frame = CurrentClip->Sample(AudioPosition); }
    return true;
}
void ULAMAudio2ExpressionComponent::PlaybackPercent(const UAudioComponent* C, const USoundWave* S, float Percent)
{
    if (C != Audio || !bPlaying || !CurrentClip || S != CurrentClip->SoundWave || bPaused) return;
    // Native notifications can be delivered after a source initialization/seek. Read the matching
    // renderer-confirmed snapshot instead of timestamping a delayed percentage as if it were fresh.
    if (!PlaybackBridge) return;
    FScopeLock Lock(&PlaybackBridge->Mutex);
    if (PlaybackBridge->ClockPercent < 0) return;
    AudioPosition = FMath::Max(AudioPosition, FMath::Clamp(PlaybackBridge->ClockPercent, 0.f, 1.f) * CurrentClip->Duration);
    LastUpdate = PlaybackBridge->ClockUpdated; bHaveClock = true;
}
void ULAMAudio2ExpressionComponent::PlaybackFinished(UAudioComponent* C) { if (C == Audio) bEngineEnded = true; }
void ULAMAudio2ExpressionComponent::Pause()
{
    if (!bPlaying || !Audio || bPaused) return;
    ++PlaybackMutation; bPaused = true; Audio->SetPaused(true); AudioPosition = Frame.TimeSeconds;
    SetPlaybackState(ELAMPlaybackState::Paused);
}
void ULAMAudio2ExpressionComponent::Resume()
{
    if (!bPlaying || !Audio || !bPaused) return;
    ++PlaybackMutation; bPaused = false; LastUpdate = FPlatformTime::Seconds(); Audio->SetPaused(false);
    SetPlaybackState(!bStartedNotified ? ELAMPlaybackState::Starting : bFadeStops ? ELAMPlaybackState::FadingOut :
        FadeGain < 1 ? ELAMPlaybackState::FadingIn : ELAMPlaybackState::Playing);
}
void ULAMAudio2ExpressionComponent::Stop()
{
    const uint64 Mutation = ++PlaybackMutation;
    EndPlayback(ELAMPlaybackEndReason::Stopped);
    if (Mutation == PlaybackMutation) StopMicrophone();
}
bool ULAMAudio2ExpressionComponent::Seek(float Time)
{
    if (!CurrentClip || !CurrentClip->SoundWave || !FMath::IsFinite(Time)) return false;
    if (!bPlaying) return PlayExpressionClipWithSettings(CurrentClip, ActivePlaybackSettings, Time);
    ++PlaybackMutation;
    if (!CreatePlaybackAudio(Time))
    { EndPlayback(ELAMPlaybackEndReason::Failed, TEXT("SeekFailed"), TEXT("Could not restart audio for the seek.")); return false; }
    return true;
}
void ULAMAudio2ExpressionComponent::SetOutputSubmix(USoundSubmixBase* S)
{ ActivePlaybackSettings.OutputSubmix = S; PlaybackSettings.OutputSubmix = S; UpdateRouting(); }
bool ULAMAudio2ExpressionComponent::SetSubmixSend(USoundSubmixBase* S, float Level)
{
    if (!IsValid(S) || !FMath::IsFinite(Level) || Level < 0 || Level > 1) return false;
    auto Set = [&](FLAMAudioPlaybackSettings& A) {
        A.AdditionalSubmixSends.RemoveAll([&](const FLAMSubmixSend& V) { return V.Submix == S; });
        FLAMSubmixSend Send; Send.Submix = S; Send.Level = Level; A.AdditionalSubmixSends.Add(Send);
    };
    Set(ActivePlaybackSettings); Set(PlaybackSettings); UpdateRouting(); return true;
}
void ULAMAudio2ExpressionComponent::RemoveSubmixSend(USoundSubmixBase* S)
{
    ActivePlaybackSettings.AdditionalSubmixSends.RemoveAll([&](const FLAMSubmixSend& V) { return V.Submix == S; });
    PlaybackSettings.AdditionalSubmixSends.RemoveAll([&](const FLAMSubmixSend& V) { return V.Submix == S; });
    UpdateRouting();
}
bool ULAMAudio2ExpressionComponent::SetVolume(float V)
{
    if (!FMath::IsFinite(V) || V < 0) return false;
    ActivePlaybackSettings.Volume = PlaybackSettings.Volume = V; UpdateRouting(); return true;
}
void ULAMAudio2ExpressionComponent::SetMuted(bool Muted)
{ ActivePlaybackSettings.bMuted = PlaybackSettings.bMuted = Muted; UpdateRouting(); }
bool ULAMAudio2ExpressionComponent::FadeOutAndStop(float Duration)
{
    if (!bPlaying || !FMath::IsFinite(Duration) || Duration < 0) return false;
    if (Duration == 0) { Stop(); return true; }
    ++PlaybackMutation; FadeElapsed = 0; FadeDuration = Duration; FadeFrom = FadeGain; FadeTo = 0; bFadeStops = true;
    if (!bPaused) SetPlaybackState(ELAMPlaybackState::FadingOut);
    return true;
}
float ULAMAudio2ExpressionComponent::GetARKitCurveValue(FName Name) const
{
    const int I = LAM::CurveNames().IndexOfByKey(Name);
    return Frame.Values.IsValidIndex(I) ? Frame.Values[I] : 0;
}
TArray<FName> ULAMAudio2ExpressionComponent::GetARKitCurveNames() { return LAM::CurveNames(); }
void ULAMAudio2ExpressionComponent::TickComponent(float Delta, ELevelTick Type, FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta, Type, Function);
    const double Now = FPlatformTime::Seconds();
    if (GetWorld()->IsPaused() && (Live || ActivePlaybackSettings.bPlayWhenGamePaused))
        Delta = LastTickRealtime > 0 ? float(Now - LastTickRealtime) : 0;
    LastTickRealtime = Now;
    if (Live) { UpdateLive(Delta); return; }
    if (bPlaying && Audio && PlaybackBridge)
    {
        const uint64 Mutation = PlaybackMutation;
        if (!bStartedNotified && PlaybackBridge->Started)
        {
            const int64 StartedId = PlaybackInfo.PlaybackId;
            bStartedNotified = true;
            SetPlaybackState(bPaused ? ELAMPlaybackState::Paused : bFadeStops ? ELAMPlaybackState::FadingOut :
                FadeGain < 1 ? ELAMPlaybackState::FadingIn : ELAMPlaybackState::Playing);
            // Pause/seek from a state listener still belongs to this accepted play.
            // Only a stop/replacement suppresses its started notification.
            if (!bPlaying || PlaybackInfo.PlaybackId != StartedId) return;
            OnPlaybackStarted.Broadcast(GetPlaybackInfo());
            if (Mutation != PlaybackMutation || !bPlaying) return;
        }
        if (bEngineEnded || PlaybackBridge->Ended || !Audio->IsPlaying())
        {
            const auto Reason = PlaybackBridge->NaturalEnd ? ELAMPlaybackEndReason::Completed :
                bStartedNotified ? ELAMPlaybackEndReason::Interrupted : ELAMPlaybackEndReason::Failed;
            EndPlayback(Reason, TEXT("StartRejected"), TEXT("The engine rejected playback (audio device or concurrency limit)."));
            return;
        }
        const bool WorldPaused = GetWorld()->IsPaused() && !ActivePlaybackSettings.bPlayWhenGamePaused;
        if (WorldPaused) { LastUpdate = FPlatformTime::Seconds(); return; }
        if (!bPaused)
        {
            FScopeLock Lock(&PlaybackBridge->Mutex);
            if (PlaybackBridge->ClockPercent >= 0)
            {
                AudioPosition = FMath::Max(AudioPosition, FMath::Clamp(PlaybackBridge->ClockPercent, 0.f, 1.f) * CurrentClip->Duration);
                LastUpdate = PlaybackBridge->ClockUpdated;
                bHaveClock = true;
            }
        }
        if (!bPaused && bStartedNotified && FadeDuration > 0)
        {
            FadeElapsed = FMath::Min(FadeElapsed + Delta, FadeDuration);
            FadeGain = FMath::Lerp(FadeFrom, FadeTo, FadeElapsed / FadeDuration);
            PlaybackBridge->Gain = ActivePlaybackSettings.bMuted ? 0 : ActivePlaybackSettings.Volume * FadeGain;
            if (FadeElapsed >= FadeDuration)
            {
                FadeDuration = 0;
                if (bFadeStops) { Stop(); return; }
                SetPlaybackState(ELAMPlaybackState::Playing);
                if (Mutation != PlaybackMutation || !bPlaying) return;
            }
        }
    }
    if (bPlaying && CurrentClip && bHaveClock)
    {
        float Time = AudioPosition;
        if (!bPaused) Time += float(FMath::Min(FPlatformTime::Seconds() - LastUpdate, 1.0 / 30.0));
        Frame = CurrentClip->Sample(FMath::Max(Time, Frame.TimeSeconds));
        PlaybackInfo.Position = Frame.TimeSeconds;
    }
    else if (!bPlaying)
    {
        Frame.Weight = FMath::Max(0.f, Frame.Weight - Delta / 0.1f);
        if (Frame.Weight == 0) Frame.bValid = false;
    }
}
void ULAMAudio2ExpressionComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    bEndingPlay = true; ++PlaybackMutation;
    CancelAnalysis(); Stop(); ReleasePlaybackAudio();
    Super::EndPlay(Reason);
}
