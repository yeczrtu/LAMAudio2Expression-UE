#include "LAMOfflineAnalysis.h"
#include "LAMCore.h"
#include "LAMSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Sound/SoundWave.h"
#include "Misc/SecureHash.h"

struct FLAMOfflineWork : FLAMJob
{
    FString PCMHash;
};

ULAMOfflineAnalysis *ULAMOfflineAnalysis::Start(USoundWave *Wave, const FLAMAnalysisSettings &Settings)
{
    check(IsInGameThread());
    auto *Task = NewObject<ULAMOfflineAnalysis>();
    Task->Sound = Wave;
    Task->Options = Settings;
    Task->Work = MakeShared<FLAMOfflineWork, ESPMode::ThreadSafe>();
    if (!IsValid(Wave) || Wave->IsProcedurallyGenerated() || Wave->bIsSourceBus || Wave->IsLooping() ||
        Wave->NumChannels < 1 || Wave->NumChannels > 2 || !FMath::IsFinite(Wave->Duration) || Wave->Duration <= 0 ||
        Wave->Duration > 300 || Settings.Style < 0 || Settings.Style > 11)
    {
        Task->Error = TEXT("Require a non-looping mono/stereo SoundWave, 0-300 seconds, and style 0-11.");
        return Task;
    }
    Task->SourceGuid = Wave->CompressedDataGuid;
    const auto Path = GetDefault<ULAMSettings>()->Model.ToSoftObjectPath();
    if (!Path.IsValid())
    {
        Task->Error = TEXT("No LAM model is configured.");
        return Task;
    }
    Task->ModelPath = Path.ToString();
    Task->LoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        Path, FStreamableDelegate::CreateUObject(Task, &ULAMOfflineAnalysis::StartWork));
    if (!Task->LoadHandle)
        Task->Error = TEXT("Cannot request the configured LAM model.");
    return Task;
}
void ULAMOfflineAnalysis::StartWork()
{
    if (bCancelled)
        return;
    if (!IsValid(Sound) || Sound->CompressedDataGuid != SourceGuid)
    {
        Error = TEXT("The source sound changed during analysis. Generate again.");
        return;
    }
    Model = Cast<UNNEModelData>(FSoftObjectPath(ModelPath).ResolveObject());
    if (!Model)
    {
        Error = TEXT("The configured LAM model is missing.");
        return;
    }
    ModelGuid = Model->GetFileId();
    if (!Sound->IsStreaming())
        Sound->InitAudioResource(Sound->GetRuntimeFormat());
    auto Wave = Sound->GetSoundWaveProxy()->GetSoundWaveDataRef();
    const auto Models = LAM::CreateModels(Model, GetDefault<ULAMSettings>()->bPreferGPU);
    if (!Models.CPU && !Models.GPU)
    {
        Error = TEXT("Neither DirectML nor CPU can load the LAM model.");
        return;
    }
    const auto Settings = Options;
    const auto Job = Work;
    LAM::Queue(
        [Wave, Models, Settings, Job]()
        {
            TArray<float> PCM;
            if (LAM::Decode(Wave, PCM, *Job) && !Job->Cancelled)
            {
                FSHA1 Hash;
                Hash.Update(reinterpret_cast<const uint8 *>(PCM.GetData()), PCM.Num() * sizeof(float));
                Hash.Final();
                uint8 Bytes[20];
                Hash.GetHash(Bytes);
                Job->PCMHash = BytesToHex(Bytes, 20);
                LAM::Analyze(PCM, Models, Settings, *Job);
            }
            Job->Done = true;
        });
}
bool ULAMOfflineAnalysis::IsDone() const
{
    return bCancelled || !Error.IsEmpty() || (Work && Work->Done);
}
float ULAMOfflineAnalysis::GetProgress() const
{
    return Work ? FMath::Clamp(Work->Progress.Load(), 0.f, 1.f) : 0;
}
void ULAMOfflineAnalysis::Cancel()
{
    bCancelled = true;
    if (Work)
        Work->Cancelled = true;
    if (LoadHandle)
        LoadHandle->CancelHandle();
}
bool ULAMOfflineAnalysis::TakeResult(FLAMOfflineResult &Result, FString &OutError)
{
    check(IsInGameThread());
    OutError.Reset();
    if (bTaken)
        OutError = TEXT("Analysis result was already consumed.");
    else if (bCancelled)
        OutError = TEXT("Cancelled.");
    else if (!IsDone())
        OutError = TEXT("Analysis is not complete.");
    else if (!Error.IsEmpty())
        OutError = Error;
    else if (!Work->Error.IsEmpty())
        OutError = Work->Error;
    else if (!IsValid(Sound) || Sound->CompressedDataGuid != SourceGuid || !Model || Model->GetFileId() != ModelGuid ||
             GetDefault<ULAMSettings>()->Model.ToSoftObjectPath().ToString() != ModelPath)
        OutError = TEXT("The source sound or model changed during analysis. Generate again.");
    if (!OutError.IsEmpty())
        return false;
    Result.Curves = MoveTemp(Work->Curves);
    Result.NumSamples = Work->NumSamples;
    Result.Duration = Work->Duration;
    Result.AnalysisSeconds = Work->Seconds;
    Result.Backend = Work->Backend;
    Result.PCMHash = Work->PCMHash;
    Result.SourceGuid = SourceGuid;
    Result.ModelGuid = ModelGuid;
    Result.ModelPath = ModelPath;
    bTaken = true;
    return true;
}
void ULAMOfflineAnalysis::BeginDestroy()
{
    Cancel();
    Super::BeginDestroy();
}
