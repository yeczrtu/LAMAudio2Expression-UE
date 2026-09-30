#pragma once
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Containers/Ticker.h"
#include "LAMTypes.h"
#include "LAMBakeSubsystem.generated.h"

USTRUCT()
struct FLAMBakeRequest
{
    GENERATED_BODY()
    UPROPERTY() TObjectPtr<class USoundWave> Sound;
    UPROPERTY() TObjectPtr<class ULAMBakedExpressionClip> Target;
    UPROPERTY() FLAMAnalysisSettings Settings;
    UPROPERTY() bool bRegenerate = false;
};

UCLASS()
class LAMAUDIO2EXPRESSIONEDITOR_API ULAMBakeOptions : public UObject
{
    GENERATED_BODY()
  public:
    UPROPERTY(EditAnywhere, Category = "Analysis") FLAMAnalysisSettings Settings;
};

UCLASS()
class LAMAUDIO2EXPRESSIONEDITOR_API ULAMBakeSubsystem : public UEditorSubsystem
{
    GENERATED_BODY()
  public:
    UFUNCTION(BlueprintCallable, Category = "LAM|Editor", meta = (Keywords = "Lipsync"))
    bool GenerateClips(const TArray<USoundWave *> &Sounds, FLAMAnalysisSettings Settings);
    UFUNCTION(BlueprintCallable, Category = "LAM|Editor", meta = (Keywords = "Lipsync"))
    bool RegenerateClips(const TArray<ULAMBakedExpressionClip *> &Clips);
    UFUNCTION(BlueprintCallable, Category = "LAM|Editor", meta = (Keywords = "Lipsync")) void Cancel();
    UFUNCTION(BlueprintPure, Category = "LAM|Editor", meta = (Keywords = "Lipsync")) bool IsBusy() const
    {
        return bBusy;
    }
    UFUNCTION(BlueprintPure, Category = "LAM|Editor", meta = (Keywords = "Lipsync")) float GetProgress() const;
    UFUNCTION(BlueprintPure, Category = "LAM|Editor", meta = (Keywords = "Lipsync")) FString GetStatus() const
    {
        return Status;
    }
    uint64 GetBatchSerial() const
    {
        return BatchSerial;
    }
    UPROPERTY(BlueprintReadOnly, Transient, Category = "LAM|Editor")
    TArray<TObjectPtr<ULAMBakedExpressionClip>> GeneratedClips;
    UPROPERTY(BlueprintReadOnly, Transient, Category = "LAM|Editor") TArray<FString> Errors;
    void Deinitialize() override;

  private:
    bool BeginBatch();
    bool Tick(float Delta);
    bool CommitResult(FString &Error);
    UPROPERTY(Transient) TArray<FLAMBakeRequest> Requests;
    UPROPERTY(Transient) TObjectPtr<class ULAMOfflineAnalysis> Job;
    FTSTicker::FDelegateHandle Ticker;
    int32 Index = 0;
    float FinalProgress = 0;
    bool bBusy = false;
    uint64 BatchSerial = 0;
    FString Status;
};
