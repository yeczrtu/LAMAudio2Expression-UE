#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LAMTypes.h"
#include "LAMPlaybackTypes.h"
#include "LAMAudio2ExpressionComponent.generated.h"
class UAudioComponent;
class ULAMAnalyzeAsync;
struct FLAMLiveSession;
struct FLAMPlaybackBridge;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLAMStatusEvent, FString, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLAMPlaybackEvent, FLAMPlaybackInfo, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLAMPlaybackEndedEvent, FLAMPlaybackInfo, Info, ELAMPlaybackEndReason, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLAMPlaybackFailedEvent, FLAMPlaybackError, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLAMLiveStateEvent, FLAMLiveMetrics, Metrics);
UCLASS(ClassGroup = (Animation), meta = (BlueprintSpawnableComponent))
class LAMAUDIO2EXPRESSION_API ULAMAudio2ExpressionComponent : public UActorComponent
{
    GENERATED_BODY()
  public:
    ULAMAudio2ExpressionComponent();
    virtual ~ULAMAudio2ExpressionComponent();
    UPROPERTY(BlueprintAssignable, Category = "LAM") FLAMStatusEvent OnStatus;
    UPROPERTY(BlueprintReadOnly, Category = "LAM") TObjectPtr<ULAMExpressionClip> CurrentClip;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") FLAMAudioPlaybackSettings PlaybackSettings;
    UPROPERTY(BlueprintAssignable, Category="LAM|Audio") FLAMPlaybackEvent OnPlaybackStarted;
    UPROPERTY(BlueprintAssignable, Category="LAM|Audio") FLAMPlaybackEvent OnPlaybackFinished;
    UPROPERTY(BlueprintAssignable, Category="LAM|Audio") FLAMPlaybackEndedEvent OnPlaybackEnded;
    UPROPERTY(BlueprintAssignable, Category="LAM|Audio") FLAMPlaybackFailedEvent OnPlaybackFailed;
    UPROPERTY(BlueprintAssignable, Category="LAM|Audio") FLAMPlaybackEvent OnPlaybackStateChanged;
    UFUNCTION(BlueprintCallable, Category="LAM|Audio") bool PlayExpressionClipWithSettings(ULAMExpressionClip* Clip, const FLAMAudioPlaybackSettings& Settings, float StartTime = 0);
    UFUNCTION(BlueprintCallable, Category="LAM|Audio") void SetOutputSubmix(USoundSubmixBase* Submix);
    UFUNCTION(BlueprintCallable, Category="LAM|Audio") bool SetSubmixSend(USoundSubmixBase* Submix, float Level);
    UFUNCTION(BlueprintCallable, Category="LAM|Audio") void RemoveSubmixSend(USoundSubmixBase* Submix);
    UFUNCTION(BlueprintCallable, Category="LAM|Audio") bool SetVolume(float Volume);
    UFUNCTION(BlueprintCallable, Category="LAM|Audio") void SetMuted(bool Muted);
    UFUNCTION(BlueprintCallable, Category="LAM|Audio") bool FadeOutAndStop(float Duration);
    UFUNCTION(BlueprintPure, Category="LAM|Audio") FLAMPlaybackInfo GetPlaybackInfo() const;
    UFUNCTION(BlueprintCallable, Category = "LAM")
    bool PlayExpressionClip(ULAMExpressionClip *Clip, float StartTime = 0);
    UFUNCTION(BlueprintCallable, Category = "LAM") void Pause();
    UFUNCTION(BlueprintCallable, Category = "LAM") void Resume();
    UFUNCTION(BlueprintCallable, Category = "LAM") void Stop();
    UFUNCTION(BlueprintCallable, Category = "LAM") bool Seek(float TimeSeconds);
    UFUNCTION(BlueprintCallable, Category = "LAM") void CancelAnalysis();
    UFUNCTION(BlueprintPure, Category = "LAM") FLAMExpressionFrame GetCurrentExpressionFrame() const
    {
        return Frame;
    }
    UFUNCTION(BlueprintPure, Category = "LAM") float GetARKitCurveValue(FName Name) const;
    UFUNCTION(BlueprintPure, Category = "LAM") static TArray<FName> GetARKitCurveNames();
    UFUNCTION(BlueprintCallable, Category = "LAM|Live")
    bool StartMicrophone(FLAMAnalysisSettings Settings, int32 DeviceIndex = -1);
    UFUNCTION(BlueprintCallable, Category = "LAM|Live") bool StartPCMStream(FLAMAnalysisSettings Settings);
    UFUNCTION(BlueprintCallable, Category = "LAM|Live") void StopMicrophone();
    UFUNCTION(BlueprintCallable, Category = "LAM|Live")
    bool PushPCMAudio(const TArray<float> &InterleavedPCM, int32 SampleRate, int32 Channels);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Live", meta = (ClampMin = "0.4", ClampMax = "2.0"))
    float PresentationDelay = 0.75f;
    UPROPERTY(BlueprintReadOnly, Category = "LAM|Live") float InferenceP95Milliseconds = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintSetter=SetLiveInferenceInterval, Category="LAM|Live", meta=(ClampMin="33.333333", ClampMax="1000"))
    float LiveInferenceIntervalMilliseconds = 1000.f / 3;
    UFUNCTION(BlueprintSetter, Category="LAM|Live") void SetLiveInferenceInterval(float Milliseconds);
    UFUNCTION(BlueprintPure, Category="LAM|Live") float GetLiveInferenceInterval() const;
    UFUNCTION(BlueprintPure, Category="LAM|Live") FLAMLiveMetrics GetLiveMetrics() const { return LiveMetrics; }
    UPROPERTY(BlueprintAssignable, Category="LAM|Live") FLAMLiveStateEvent OnLiveStateChanged;
    void SetAnalysis(ULAMAnalyzeAsync *Action);

  protected:
    void EndPlay(const EEndPlayReason::Type Reason) override;
    void TickComponent(float Delta, ELevelTick TickType, FActorComponentTickFunction *ThisTick) override;

  private:
    void PlaybackPercent(const UAudioComponent *Component, const USoundWave *Sound, float Percent);
    void PlaybackFinished(UAudioComponent *Component);
    void UpdateLive(float Delta);
    void SetLiveState(ELAMLiveState State);
    bool CreatePlaybackAudio(float StartTime);
    void ReleasePlaybackAudio();
    void EndPlayback(ELAMPlaybackEndReason Reason, FName ErrorCode = NAME_None, const FString& Message = FString());
    void SetPlaybackState(ELAMPlaybackState State);
    void UpdateRouting();
    void ReportPlaybackError(ULAMExpressionClip* Clip, int64 Id, FName Code, const FString& Message);
    UPROPERTY(Transient) TObjectPtr<UAudioComponent> Audio;
    UPROPERTY(Transient) TObjectPtr<ULAMAnalyzeAsync> Analysis;
    UPROPERTY(Transient) TObjectPtr<class UNNEModelData> LiveModel;
    UPROPERTY(Transient) FLAMAudioPlaybackSettings ActivePlaybackSettings;
    UPROPERTY(Transient) FLAMPlaybackInfo PlaybackInfo;
    FLAMLiveMetrics LiveMetrics;
    TSharedPtr<FLAMPlaybackBridge, ESPMode::ThreadSafe> PlaybackBridge;
    int64 NextPlaybackId = 0;
    uint64 PlaybackMutation = 0;
    bool bEndingPlay = false, bStartedNotified = false, bEngineEnded = false;
    float FadeElapsed = 0, FadeDuration = 0, FadeFrom = 1, FadeTo = 1, FadeGain = 1;
    bool bFadeStops = false;
    FLAMExpressionFrame Frame;
    double LastUpdate = 0, LastTickRealtime = 0;
    float AudioPosition = 0;
    bool bPlaying = false, bPaused = false, bHaveClock = false;
    TSharedPtr<FLAMLiveSession, ESPMode::ThreadSafe> Live;
    TSharedPtr<class FLAMCapture> Capture;
};
