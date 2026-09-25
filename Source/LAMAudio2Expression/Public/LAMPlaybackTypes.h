#pragma once
#include "CoreMinimal.h"
#include "LAMTypes.h"
#include "LAMPlaybackTypes.generated.h"

class USoundSubmixBase;
class USoundClass;
class USoundAttenuation;
class USoundConcurrency;
class USceneComponent;

UENUM(BlueprintType)
enum class ELAMPlaybackMode : uint8 { TwoDimensional, ThreeDimensional };
UENUM(BlueprintType)
enum class ELAMPlaybackState : uint8 { Stopped, Starting, Playing, Paused, FadingIn, FadingOut, Failed };
UENUM(BlueprintType)
enum class ELAMPlaybackEndReason : uint8 { Completed, Stopped, Replaced, Interrupted, Failed };
UENUM(BlueprintType)
enum class ELAMLiveState : uint8 { Stopped, Preparing, Running, Lagging, Failed };

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMSubmixSend
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") TObjectPtr<USoundSubmixBase> Submix;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio", meta=(ClampMin="0", ClampMax="1")) float Level = 1;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMAudioPlaybackSettings
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") TObjectPtr<USoundSubmixBase> OutputSubmix;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") TArray<FLAMSubmixSend> AdditionalSubmixSends;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") bool bInheritSoundWaveSends = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") TObjectPtr<USoundClass> SoundClassOverride;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio", meta=(ClampMin="0")) float Volume = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") bool bMuted = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") ELAMPlaybackMode PlaybackMode = ELAMPlaybackMode::TwoDimensional;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") TObjectPtr<USceneComponent> Attachment;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") FName Socket;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") FTransform Transform = FTransform::Identity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") TObjectPtr<USoundAttenuation> AttenuationSettings;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") TSet<TObjectPtr<USoundConcurrency>> ConcurrencySettings;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio") bool bPlayWhenGamePaused = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LAM|Audio", meta=(ClampMin="0")) float FadeInDuration = 0;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMPlaybackInfo
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") int64 PlaybackId = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") TObjectPtr<ULAMExpressionClip> Clip;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") ELAMPlaybackState State = ELAMPlaybackState::Stopped;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") float Position = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") float Duration = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") float Progress = 0;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMPlaybackError
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") int64 PlaybackId = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") TObjectPtr<ULAMExpressionClip> Clip;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") FName Code;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Audio") FString Message;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMLiveMetrics
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") ELAMLiveState State = ELAMLiveState::Stopped;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") float ActualIntervalMilliseconds = 1000.f / 3;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") float EffectivePresentationDelayMilliseconds = 750;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") float InferenceP95Milliseconds = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") float ResultLatencyP95Milliseconds = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") float InitializationMilliseconds = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") int64 DroppedIntervals = 0;
    UPROPERTY(BlueprintReadOnly, Category="LAM|Live") FString Backend;
};
