#include "LAMBakeSubsystem.h"
#include "LAMBakedExpressionClip.h"
#include "LAMOfflineAnalysis.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "Misc/PackageName.h"
#include "ScopedTransaction.h"
#include "Sound/SoundWave.h"
#include "UObject/Package.h"

bool ULAMBakeSubsystem::GenerateClips(const TArray<USoundWave *> &Sounds, FLAMAnalysisSettings Settings)
{
    if (bBusy || Sounds.IsEmpty() || (GEditor && GEditor->PlayWorld))
        return false;
    Requests.Reset();
    TSet<USoundWave *> Seen;
    for (auto *Sound : Sounds)
        if (!Seen.Contains(Sound))
        {
            Seen.Add(Sound);
            auto &R = Requests.AddDefaulted_GetRef();
            R.Sound = Sound;
            R.Settings = Settings;
        }
    return BeginBatch();
}
bool ULAMBakeSubsystem::RegenerateClips(const TArray<ULAMBakedExpressionClip *> &Clips)
{
    if (bBusy || Clips.IsEmpty() || (GEditor && GEditor->PlayWorld))
        return false;
    Requests.Reset();
    TSet<ULAMBakedExpressionClip *> Seen;
    for (auto *Clip : Clips)
        if (IsValid(Clip) && !Seen.Contains(Clip))
        {
            Seen.Add(Clip);
            auto &R = Requests.AddDefaulted_GetRef();
            R.Sound = Clip->SoundWave;
            R.Target = Clip;
            R.Settings = Clip->Settings;
            R.bRegenerate = true;
        }
    return !Requests.IsEmpty() && BeginBatch();
}
bool ULAMBakeSubsystem::BeginBatch()
{
    GeneratedClips.Reset();
    Errors.Reset();
    Index = 0;
    FinalProgress = 0;
    bBusy = true;
    ++BatchSerial;
    Status = TEXT("Queued");
    Ticker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ULAMBakeSubsystem::Tick));
    return true;
}
float ULAMBakeSubsystem::GetProgress() const
{
    return Requests.IsEmpty() ? FinalProgress : (Index + (Job ? Job->GetProgress() : 0.f)) / Requests.Num();
}
void ULAMBakeSubsystem::Cancel()
{
    FinalProgress = GetProgress();
    if (Job)
        Job->Cancel();
    Job = nullptr;
    if (bBusy)
        Status = TEXT("Cancelled. Completed clips remain available for saving.");
    bBusy = false;
    FTSTicker::GetCoreTicker().RemoveTicker(Ticker);
    Requests.Reset();
}
void ULAMBakeSubsystem::Deinitialize()
{
    Cancel();
    Super::Deinitialize();
}
bool ULAMBakeSubsystem::Tick(float)
{
    if (!bBusy)
        return false;
    if (GEditor && GEditor->PlayWorld)
    {
        Cancel();
        return false;
    }
    if (Index == Requests.Num())
    {
        Status = FString::Printf(TEXT("Finished: %d clips, %d failures. Save generated assets to keep them."),
                                 GeneratedClips.Num(), Errors.Num());
        UE_LOG(LogTemp, Display, TEXT("LAM Bake: %s"), *Status);
        bBusy = false;
        FinalProgress = 1;
        Requests.Reset();
        return false;
    }
    const auto &R = Requests[Index];
    if (!Job)
    {
        Status = FString::Printf(TEXT("%d/%d: %s"), Index + 1, Requests.Num(), *GetNameSafe(R.Sound));
        Job = ULAMOfflineAnalysis::Start(R.Sound, R.Settings);
    }
    if (!Job->IsDone())
        return true;
    FString Error;
    if (!CommitResult(Error))
    {
        Error = GetPathNameSafe(R.Sound) + TEXT(": ") + Error;
        Errors.Add(Error);
        UE_LOG(LogTemp, Warning, TEXT("LAM Bake: %s"), *Error);
    }
    Job = nullptr;
    ++Index;
    return true;
}
bool ULAMBakeSubsystem::CommitResult(FString &Error)
{
    FLAMOfflineResult Result;
    if (!Job->TakeResult(Result, Error))
        return false;
    const auto &R = Requests[Index];
    if (R.bRegenerate && !IsValid(R.Target))
    {
        Error = TEXT("The target clip was deleted during analysis.");
        return false;
    }
    if (R.Target && (R.Target->SoundWave != R.Sound ||
                     !FLAMAnalysisSettings::StaticStruct()->CompareScriptStruct(&R.Target->Settings, &R.Settings, 0)))
    {
        Error = TEXT("The target clip changed during analysis. Generate again.");
        return false;
    }
    auto Fill = [&](ULAMBakedExpressionClip *Clip)
    {
        Clip->SoundWave = R.Sound;
        Clip->Settings = R.Settings;
        Clip->Duration = Result.Duration;
        Clip->FrameRate = LAM::FPS;
        Clip->NumSamples = Result.NumSamples;
        Clip->Backend = Result.Backend;
        Clip->AnalysisSeconds = Result.AnalysisSeconds;
        Clip->FormatVersion = ULAMBakedExpressionClip::CurrentFormatVersion;
        Clip->ProcessingVersion = ULAMBakedExpressionClip::CurrentProcessingVersion;
        Clip->SourceGuid = Result.SourceGuid;
        Clip->ModelGuid = Result.ModelGuid;
        Clip->ModelPath = Result.ModelPath;
        Clip->PCMHash = Result.PCMHash;
        Clip->Curves = MoveTemp(Result.Curves);
        Clip->RefreshValidation();
    };
    // Validate in temporary storage before touching any existing asset.
    auto *Candidate = NewObject<ULAMBakedExpressionClip>();
    Fill(Candidate);
    Error = Candidate->GetPlaybackValidationError();
    if (!Error.IsEmpty())
        return false;
    Result.Curves = MoveTemp(Candidate->Curves);
    ULAMBakedExpressionClip *Target = R.Target;
    FString PackageName, AssetName;
    if (!Target)
    {
        const FString Base = R.Sound->GetOutermost()->GetName() + TEXT("_LAMClip");
        if (!FPackageName::IsValidLongPackageName(Base) || R.Sound->GetOutermost() == GetTransientPackage())
        {
            Error = TEXT("Source sound must be a content asset.");
            return false;
        }
        for (int32 Suffix = 0;; ++Suffix)
        {
            PackageName = Base + (Suffix ? FString::Printf(TEXT("_%d"), Suffix) : FString());
            AssetName = FPackageName::GetShortName(PackageName);
            const FString ObjectPath = PackageName + TEXT(".") + AssetName;
            auto *Existing = FindObject<UObject>(nullptr, *ObjectPath);
            const bool bExistsOnDisk = FPackageName::DoesPackageExist(PackageName);
            if (!Existing && bExistsOnDisk)
                Existing = LoadObject<UObject>(nullptr, *ObjectPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
            if (auto *Baked = Cast<ULAMBakedExpressionClip>(Existing); Baked && Baked->SoundWave == R.Sound)
            {
                Target = Baked;
                break;
            }
            if (!Existing && !bExistsOnDisk && !FindPackage(nullptr, *PackageName))
                break;
        }
    }
    const FScopedTransaction Transaction(NSLOCTEXT("LAM", "BakeTransaction", "Generate LAM Expression Clip"));
    const bool bNew = !Target;
    if (bNew)
        Target = NewObject<ULAMBakedExpressionClip>(CreatePackage(*PackageName), *AssetName,
                                                    RF_Public | RF_Standalone | RF_Transactional);
    Target->Modify();
    Fill(Target);
    if (bNew)
        FAssetRegistryModule::AssetCreated(Target);
    Target->MarkPackageDirty();
    GeneratedClips.Add(Target);
    return true;
}
