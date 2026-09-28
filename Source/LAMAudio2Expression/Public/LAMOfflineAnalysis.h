#pragma once
#include "CoreMinimal.h"
#include "LAMTypes.h"
#include "LAMOfflineAnalysis.generated.h"

struct FLAMOfflineResult
{
    TArray<float> Curves;
    int32 NumSamples = 0;
    float Duration = 0, AnalysisSeconds = 0;
    FString Backend, PCMHash, ModelPath;
    FGuid SourceGuid, ModelGuid;
};

// World-independent entry point for editor baking. Keep the job referenced until completion.
UCLASS()
class LAMAUDIO2EXPRESSION_API ULAMOfflineAnalysis : public UObject
{
    GENERATED_BODY()
  public:
    static ULAMOfflineAnalysis *Start(class USoundWave *Sound, const FLAMAnalysisSettings &Settings);
    bool IsDone() const;
    float GetProgress() const;
    void Cancel();
    bool TakeResult(FLAMOfflineResult &Result, FString &Error);
    void BeginDestroy() override;

  private:
    void StartWork();
    UPROPERTY() TObjectPtr<class USoundWave> Sound;
    UPROPERTY() TObjectPtr<class UNNEModelData> Model;
    FLAMAnalysisSettings Options;
    TSharedPtr<struct FStreamableHandle> LoadHandle;
    TSharedPtr<struct FLAMOfflineWork, ESPMode::ThreadSafe> Work;
    FString Error, ModelPath;
    FGuid SourceGuid, ModelGuid;
    bool bCancelled = false, bTaken = false;
};
