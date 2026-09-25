// SPDX-License-Identifier: Apache-2.0
// Adapted from aigc3d/LAM_Audio2Expression (02a703c3ea7d8e360eb43098eca85ee98a083529).
// Modified 2026: UE C++ runtime integration, deterministic timing and optional postprocessing.
// See ../../../Licenses/Apache-2.0.txt and ../../../THIRD_PARTY_NOTICES.md.
#include "LAMLive.h"
#include "LAMAudio2ExpressionComponent.h"
#include "LAMSettings.h"
#include "Components/AudioComponent.h"
#include "Algo/Sort.h"
#include "LAMLiveTiming.h"

bool ULAMAudio2ExpressionComponent::StartPCMStream(FLAMAnalysisSettings Settings)
{
    if (bEndingPlay) return false;
    const uint64 Mutation = ++PlaybackMutation;
    EndPlayback(ELAMPlaybackEndReason::Replaced);
    if (Mutation != PlaybackMutation || bEndingPlay) return false;
    StopMicrophone();
    if (Mutation != PlaybackMutation || bEndingPlay) return false;
    if (Settings.Style < 0 || Settings.Style > 11)
    { SetLiveState(ELAMLiveState::Failed); return false; }
    const double ModelStart = FPlatformTime::Seconds();
    LiveModel = GetDefault<ULAMSettings>()->Model.LoadSynchronous();
    auto Models = LAM::CreateModels(LiveModel, GetDefault<ULAMSettings>()->bPreferGPU);
    if (!Models.CPU && !Models.GPU)
    {
        SetLiveState(ELAMLiveState::Failed);
        OnStatus.Broadcast(TEXT("Live input requires an imported and cooked LAM model."));
        return false;
    }
    Live = MakeShared<FLAMLiveSession, ESPMode::ThreadSafe>();
    Live->Models = Models;
    Live->GPU = Models.GPU.IsValid();
    Live->Options = Settings;
    Live->StartClock = FPlatformTime::Seconds();
    Live->InitializationMs = float((FPlatformTime::Seconds() - ModelStart) * 1000);
    Live->AppliedHop = LAM::LiveHop(LiveInferenceIntervalMilliseconds);
    LiveMetrics = FLAMLiveMetrics();
    LiveMetrics.ActualIntervalMilliseconds = GetLiveInferenceInterval();
    LiveMetrics.EffectivePresentationDelayMilliseconds = LAM::LiveDelay(PresentationDelay, 0, GetLiveInferenceInterval(), 0);
    CurrentClip = nullptr;
    Frame = FLAMExpressionFrame();
    InferenceP95Milliseconds = 0;
    SetLiveState(ELAMLiveState::Preparing);
    if (PlaybackMutation != Mutation || !Live) return false;
    const auto Session = Live;
    OnStatus.Broadcast(TEXT("Live input started; microphone monitoring is disabled."));
    return Live == Session;
}
bool ULAMAudio2ExpressionComponent::StartMicrophone(FLAMAnalysisSettings Settings, int32 Index)
{
    if (!StartPCMStream(Settings))
        return false;
    Capture = MakeShared<FLAMCapture>();
    Audio::FAudioCaptureDeviceParams Params;
    Params.DeviceIndex = Index;
    auto Session = Live;
    if (!Capture->Device.OpenAudioCaptureStream(
            Params,
            [Session](const void *Data, int32 Frames, int32 Channels, int32 Rate, double, bool Overflow)
            {
                if (Session->Cancelled || Channels < 1 || Channels > 2 || Rate <= 0)
                    return;
                FScopeLock Lock(&Session->Mutex);
                Session->CaptureRate = Rate;
                Session->CaptureChannels = Channels;
                const int Count = Frames * Channels, Limit = Rate * Channels * 2;
                if (Count <= 0 || Count > Limit)
                {
                    Session->CaptureOverflow = true;
                    return;
                }
                if (Session->CapturePCM.Num() + Count > Limit)
                {
                    Session->CapturePCM.RemoveAt(0, Session->CapturePCM.Num() + Count - Limit, EAllowShrinking::No);
                    Session->CaptureOverflow = true;
                }
                Session->CapturePCM.Append(static_cast<const float *>(Data), Count);
                Session->CaptureOverflow |= Overflow;
            },
            1024) ||
        !Capture->Device.StartStream())
    {
        StopMicrophone();
        SetLiveState(ELAMLiveState::Failed);
        OnStatus.Broadcast(TEXT("Microphone unavailable or access denied."));
        return false;
    }
    return true;
}
void ULAMAudio2ExpressionComponent::StopMicrophone()
{
    if (Live)
        Live->Cancelled = true;
    Capture.Reset();
    Live.Reset();
    LiveModel = nullptr;
    SetLiveState(ELAMLiveState::Stopped);
}
bool ULAMAudio2ExpressionComponent::PushPCMAudio(const TArray<float> &PCM, int32 Rate, int32 Channels)
{
    if (!Live || Rate < 8000 || Rate > 192000 || Channels < 1 || Channels > 2 || PCM.IsEmpty() ||
        PCM.Num() % Channels || PCM.Num() > Rate * Channels * 2)
        return false;
    auto &S = *Live;
    if (S.NativeRate && S.NativeRate != Rate)
    {
        OnStatus.Broadcast(TEXT("Sample rate changed: restart the PCM stream."));
        return false;
    }
    for (float V : PCM)
        if (!FMath::IsFinite(V))
            return false;
    S.NativeRate = Rate;
    for (int I = 0; I < PCM.Num(); I += Channels)
    {
        float V = 0;
        for (int C = 0; C < Channels; ++C)
            V += PCM[I + C];
        S.Native.Add(FMath::Clamp(V / Channels, -1.f, 1.f));
    }
    S.NativeTotal += PCM.Num() / Channels;
    // Continuous sample counter + symmetric sinc kernel across callback boundaries.
    const double Cutoff = FMath::Min(1.0, 16000.0 / Rate) * 0.94;
    constexpr int Radius = 32;
    while (double(S.ResampledTotal) * Rate / 16000 + Radius < S.NativeTotal)
    {
        const double Pos = double(S.ResampledTotal) * Rate / 16000;
        const int64 Center = int64(Pos);
        double Sum = 0, Weight = 0;
        for (int J = -Radius + 1; J <= Radius; ++J)
        {
            const double D = Pos - (Center + J);
            if (FMath::Abs(D) >= Radius)
                continue;
            const double X = PI * D * Cutoff;
            const double W =
                Cutoff * (FMath::Abs(X) < 1e-10 ? 1 : FMath::Sin(X) / X) * (0.5 + 0.5 * FMath::Cos(PI * D / Radius));
            const int Idx = int(FMath::Clamp<int64>(Center + J - S.NativeStart, 0, S.Native.Num() - 1));
            Sum += S.Native[Idx] * W;
            Weight += W;
        }
        S.History.Add(float(Sum / Weight));
        ++S.ResampledTotal;
        ++S.TotalSamples;
    }
    const int64 RetainFrom = FMath::Max<int64>(0, int64(double(S.ResampledTotal) * Rate / 16000) - Radius);
    const int Drop = int(RetainFrom - S.NativeStart);
    if (Drop > 0)
    {
        S.Native.RemoveAt(0, Drop, EAllowShrinking::No);
        S.NativeStart = RetainFrom;
    }
    const int MaxHistory = LAM::Window + LAM::Rate * 2;
    if (S.History.Num() > MaxHistory)
    {
        const int N = S.History.Num() - MaxHistory;
        S.History.RemoveAt(0, N, EAllowShrinking::No);
        S.HistoryStart += N;
    }
    return true;
}

void ULAMAudio2ExpressionComponent::SetLiveInferenceInterval(float MS)
{
    if (FMath::IsFinite(MS)) LiveInferenceIntervalMilliseconds = LAM::LiveHop(MS) * 1000.f / LAM::FPS;
}
float ULAMAudio2ExpressionComponent::GetLiveInferenceInterval() const
{
    return LAM::LiveHop(LiveInferenceIntervalMilliseconds) * 1000.f / LAM::FPS;
}
void ULAMAudio2ExpressionComponent::SetLiveState(ELAMLiveState State)
{
    if (LiveMetrics.State == State) return;
    LiveMetrics.State = State;
    if (!bEndingPlay) OnLiveStateChanged.Broadcast(LiveMetrics);
}
void ULAMAudio2ExpressionComponent::UpdateLive(float Delta)
{
    auto S = Live;
    if (!S) return;
    TArray<float> Input;
    int Rate = 0, Channels = 0;
    bool Overflow = false;
    {
        FScopeLock Lock(&S->Mutex);
        Swap(Input, S->CapturePCM);
        Rate = S->CaptureRate; Channels = S->CaptureChannels;
        Overflow = S->CaptureOverflow; S->CaptureOverflow = false;
    }
    if (Overflow)
    {
        {
            FScopeLock Lock(&S->Mutex);
            ++S->Generation; ++S->Dropped; S->Result.Reset(); S->Ready = false;
        }
        // Preserve the absolute session timeline while restarting the resampler at the retained input.
        const double InputSeconds = double(Input.Num()) / FMath::Max(1, Rate * Channels);
        S->TotalSamples = FMath::Max(S->TotalSamples, int64(FMath::Max(0.0, FPlatformTime::Seconds() - S->StartClock - InputSeconds) * LAM::Rate));
        S->History.Reset(); S->Native.Reset(); S->HistoryStart = S->TotalSamples;
        S->NativeStart = S->NativeTotal = S->ResampledTotal = 0; S->NativeRate = 0;
        CurrentClip = nullptr;
    }
    if (!Input.IsEmpty()) PushPCMAudio(Input, Rate, Channels);
    if (Live != S) return;
    FString Status;
    bool Failed = false, HasTimings = false;
    {
        FScopeLock Lock(&S->Mutex);
        Status = MoveTemp(S->Error); S->Error.Reset(); Failed = S->Failed;
        if (S->Ready)
        {
            if (!CurrentClip || CurrentClip->SoundWave) CurrentClip = NewObject<ULAMExpressionClip>(this);
            CurrentClip->Duration = S->Result.Num() / (52.f * 30);
            CurrentClip->Curves = S->Result;
            AudioPosition = float(S->ResultStart); S->Ready = false;
        }
        InferenceP95Milliseconds = LAM::LiveP95(S->Timings);
        LiveMetrics.InferenceP95Milliseconds = InferenceP95Milliseconds;
        LiveMetrics.ResultLatencyP95Milliseconds = LAM::LiveP95(S->Latencies);
        LiveMetrics.InitializationMilliseconds = S->InitializationMs;
        LiveMetrics.ActualIntervalMilliseconds = S->AppliedHop * 1000.f / 30;
        LiveMetrics.DroppedIntervals = S->Dropped;
        LiveMetrics.Backend = S->Backend;
        HasTimings = !S->Timings.IsEmpty();
    }
    LiveMetrics.EffectivePresentationDelayMilliseconds = LAM::LiveDelay(PresentationDelay,
        LiveMetrics.EffectivePresentationDelayMilliseconds, FMath::Max(LiveMetrics.ActualIntervalMilliseconds, GetLiveInferenceInterval()), LiveMetrics.ResultLatencyP95Milliseconds);
    if (!Status.IsEmpty()) { OnStatus.Broadcast(Status); if (Live != S) return; }
    if (Failed) { SetLiveState(ELAMLiveState::Failed); if (Live != S) return; }
    const double Candidate = FPlatformTime::Seconds() - S->StartClock - LiveMetrics.EffectivePresentationDelayMilliseconds / 1000.0;
    const double Time = FMath::Max(S->LastPresented, Candidate);
    bool Available = false;
    if (Time >= 0 && CurrentClip && !CurrentClip->SoundWave && !CurrentClip->Curves.IsEmpty())
    {
        const double Relative = Time - AudioPosition;
        Available = Relative >= 0 && Relative < CurrentClip->Curves.Num() / (52.0 * 30);
        if (Available)
        {
            Frame = CurrentClip->Sample(float(Relative));
            Frame.TimeSeconds = float(Time);
            S->LastPresented = Time; S->MissingSeconds = 0;
        }
    }
    if (!Available && Candidate >= 0)
    {
        const float Previous = S->MissingSeconds;
        S->MissingSeconds += Delta;
        const float FadeDelta = FMath::Max(0.f, S->MissingSeconds - .1f) - FMath::Max(0.f, Previous - .1f);
        Frame.Weight = FMath::Max(0.f, Frame.Weight - FadeDelta / .1f);
        if (Frame.Weight == 0) Frame.bValid = false;
    }
    if (!Failed)
    {
        const bool Slow = HasTimings && (InferenceP95Milliseconds >= LiveMetrics.ActualIntervalMilliseconds ||
            LiveMetrics.ActualIntervalMilliseconds + LiveMetrics.ResultLatencyP95Milliseconds + 1000.f / 30 > 2000);
        SetLiveState(!HasTimings ? ELAMLiveState::Preparing : Slow || (Candidate >= 0 && !Available) ? ELAMLiveState::Lagging :
            Available ? ELAMLiveState::Running : ELAMLiveState::Preparing);
        if (Live != S) return;
    }
    const int Hop = LAM::LiveHop(LiveInferenceIntervalMilliseconds);
    const int64 Next = LAM::NextLiveFrame(S->TotalSamples, S->Step, Hop);
    {
        FScopeLock Lock(&S->Mutex);
        if (S->Busy || S->Failed || Next <= S->Step) return;
        S->Busy = true; S->AppliedHop = Hop;
        S->Dropped += FMath::Max<int64>(0, (Next - S->Step) / Hop - 1);
    }
    const int64 End = Next * LAM::Rate / LAM::FPS;
    S->Step = Next;
    const uint64 Generation = S->Generation;
    const double QueuedAt = FPlatformTime::Seconds();
    TArray<float> WindowData; WindowData.SetNumZeroed(LAM::Window);
    for (int I = 0; I < LAM::Window; ++I)
    {
        const int64 P = End - LAM::Window + I - S->HistoryStart;
        if (P >= 0 && P < S->History.Num()) WindowData[I] = S->History[int(P)];
    }
    LAM::Queue([S, WindowData = MoveTemp(WindowData), Next, Hop, Generation, QueuedAt]()
    {
        const double Start = FPlatformTime::Seconds();
        {
            FScopeLock Lock(&S->Mutex);
            if (S->Cancelled) return;
            if (S->Generation != Generation || Start - QueuedAt > 2)
            { S->Busy = false; ++S->Dropped; return; }
        }
        TArray<float> Output;
        float InitMs = 0;
        const bool OK = LAM::InferWindow(WindowData, S->Options.Style, S->Models, S->Instance, S->GPU, Output, &InitMs);
        if (OK)
        {
            auto Options = S->Options; Options.bAutoBlink = false;
            LAM::Postprocess(Output, WindowData, Options, 64);
            if (S->Options.bAutoBlink)
            {
                FRandomStream Random(S->Options.BlinkSeed);
                const float Blink[] = {0, .557f, .953f, .942f, .426f, .148f, .018f};
                for (int64 F = Random.RandRange(60, 150); F < Next; F += Random.RandRange(60, 150))
                    for (int J = 0; J < 7; ++J)
                    {
                        const int64 Local = F + J - (Next - Hop);
                        if (Local >= 0 && Local < Hop)
                            Output[(64 - Hop + int(Local)) * 52 + 8] = Output[(64 - Hop + int(Local)) * 52 + 9] = Blink[J];
                    }
            }
        }
        const double Done = FPlatformTime::Seconds();
        FScopeLock Lock(&S->Mutex);
        if (S->Cancelled) return;
        S->InitializationMs += InitMs;
        if (S->Generation != Generation) { S->Busy = false; return; }
        if (OK)
        {
            const double Begin = double(Next - Hop) / 30;
            const int Existing = S->Result.Num() / 52;
            if (Existing && FMath::Abs(S->ResultStart + Existing / 30.0 - Begin) > .00001) S->Result.Reset();
            if (S->Result.IsEmpty()) S->ResultStart = Begin;
            S->Result.Append(Output.GetData() + (64 - Hop) * 52, Hop * 52);
            if (S->Result.Num() > 60 * 52)
            {
                const int Remove = S->Result.Num() - 60 * 52;
                S->Result.RemoveAt(0, Remove, EAllowShrinking::No);
                S->ResultStart += double(Remove / 52) / 30;
            }
            S->Ready = true;
            S->Backend = S->GPU ? TEXT("DirectML") : TEXT("CPU");
            S->Timings.Add(FMath::Max(0.f, float((Done - Start) * 1000) - InitMs));
            S->Latencies.Add(FMath::Max(0.f, float((Done - QueuedAt) * 1000) - InitMs));
            if (S->Timings.Num() > 60) S->Timings.RemoveAt(0);
            if (S->Latencies.Num() > 60) S->Latencies.RemoveAt(0);
        }
        else { S->Failed = true; S->Error = TEXT("Live inference failed on GPU and CPU."); }
        S->Busy = false;
    });
}
