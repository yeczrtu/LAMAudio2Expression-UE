#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LAMBakeSubsystem.h"
#include "LAMBakedExpressionClip.h"
#include "LAMOfflineAnalysis.h"
#include "LAMAnalyzeAsync.h"
#include "LAMAudio2ExpressionComponent.h"
#include "LAMTestReceiver.h"
#include "LAMSettings.h"
#include "Editor.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/PackageName.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Sound/SoundWave.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"

class FLAMBakeIntegration : public IAutomationLatentCommand
{
    FAutomationTestBase *Test;
    double Start = FPlatformTime::Seconds();
    int32 Stage = 0;
    bool PreferGPU = GetDefault<ULAMSettings>()->bPreferGPU;
    TStrongObjectPtr<ULAMBakedExpressionClip> Clip;
    TStrongObjectPtr<ULAMAudio2ExpressionComponent> Component;
    TStrongObjectPtr<ULAMAnalyzeAsync> Dynamic;
    TStrongObjectPtr<ULAMTestReceiver> Receiver;
    TStrongObjectPtr<ULAMOfflineAnalysis> Offline;
    TStrongObjectPtr<USoundWave> ChangedSound;
    TStrongObjectPtr<ULAMCurveProfile> Collision;
    TStrongObjectPtr<ULAMBakedExpressionClip> CollisionClip;
    TArray<float> Original;

  public:
    explicit FLAMBakeIntegration(FAutomationTestBase *T) : Test(T) {}
    ~FLAMBakeIntegration()
    {
        GetMutableDefault<ULAMSettings>()->bPreferGPU = PreferGPU;
        if (Dynamic)
            Dynamic->Cancel();
        if (Offline)
            Offline->Cancel();
        if (GEditor)
            GEditor->GetEditorSubsystem<ULAMBakeSubsystem>()->Cancel();
    }
    bool Update() override
    {
        auto *Baker = GEditor->GetEditorSubsystem<ULAMBakeSubsystem>();
        if (FPlatformTime::Seconds() - Start > 240)
        {
            Test->AddError(TEXT("Bake integration timed out"));
            return true;
        }
        if (Stage == 0)
        {
            GetMutableDefault<ULAMSettings>()->bPreferGPU = false;
            TArray<USoundWave *> Sounds;
            for (const TCHAR *Name : {TEXT("speech_stream"), TEXT("short_inline"), TEXT("fraction_stream"),
                                      TEXT("silence_inline"), TEXT("one_inline"), TEXT("inline_concurrency")})
            {
                auto *Sound =
                    LoadObject<USoundWave>(nullptr, *(FString(TEXT("/Game/Audio/")) + Name + TEXT(".") + Name));
                if (!Test->TestNotNull(TEXT("Bake fixture sound"), Sound))
                    return true;
                Sounds.Add(Sound);
            }
            Sounds.Add(nullptr);
            Test->TestTrue(TEXT("Batch starts"), Baker->GenerateClips(Sounds, {}));
            Test->TestFalse(TEXT("Concurrent batch rejected"), Baker->GenerateClips(Sounds, {}));
            Stage = 1;
            return false;
        }
        if (Stage == 1)
        {
            if (Baker->IsBusy())
                return false;
            if (!Test->TestEqual(TEXT("Six clips generated"), Baker->GeneratedClips.Num(), 6))
                return true;
            Test->TestEqual(TEXT("Invalid item reported separately"), Baker->Errors.Num(), 1);
            for (const auto &BakedPtr : Baker->GeneratedClips)
            {
                auto *Baked = BakedPtr.Get();
                Test->TestTrue(TEXT("Validated generated data"), Baked->GetPlaybackValidationError().IsEmpty());
                FString Error;
                Test->TestFalse(TEXT("Fresh bake"), Baked->NeedsRegeneration(Error));
                Test->TestEqual(TEXT("PCM hash stored"), Baked->PCMHash.Len(), 40);
                if (FParse::Param(FCommandLine::Get(), TEXT("LAMBakeSaveFixtures")))
                {
                    FSavePackageArgs Args;
                    Args.TopLevelFlags = RF_Public | RF_Standalone;
                    const auto File = FPackageName::LongPackageNameToFilename(Baked->GetOutermost()->GetName(),
                                                                              FPackageName::GetAssetPackageExtension());
                    Test->TestTrue(TEXT("Save fixture"),
                                   UPackage::SavePackage(Baked->GetOutermost(), Baked, *File, Args));
                }
            }
            Clip.Reset(Baker->GeneratedClips[0]);
            Original = Clip->Curves;
            Component.Reset(
                NewObject<ULAMAudio2ExpressionComponent>(GEditor->GetEditorWorldContext().World()->GetWorldSettings()));
            Receiver.Reset(NewObject<ULAMTestReceiver>());
            Dynamic.Reset(ULAMAnalyzeAsync::AnalyzeSoundWaveAsync(Component.Get(), Clip->SoundWave, Clip->Settings));
            Dynamic->Completed.AddDynamic(Receiver.Get(), &ULAMTestReceiver::Completed);
            Dynamic->Failed.AddDynamic(Receiver.Get(), &ULAMTestReceiver::Failed);
            Dynamic->Activate();
            Stage = 2;
            return false;
        }
        if (Stage == 2)
        {
            if (Receiver->Failures)
            {
                Test->AddError(TEXT("Dynamic comparison failed"));
                return true;
            }
            if (!Receiver->Completions)
                return false;
            const auto *Reference = Receiver->LastClip.Get();
            if (!Test->TestEqual(TEXT("Dynamic and baked dimensions"), Reference->Curves.Num(), Original.Num()))
                return true;
            float MaxError = 0;
            for (int32 I = 0; I < Original.Num(); ++I)
                MaxError = FMath::Max(MaxError, FMath::Abs(Original[I] - Reference->Curves[I]));
            Test->TestTrue(TEXT("Dynamic and baked CPU curves match within 1e-5"), MaxError <= 1.e-5f);
            Test->AddInfo(FString::Printf(TEXT("Baked/dynamic max_error=%.9f"), MaxError));
            Test->TestTrue(TEXT("Regenerate starts"), Baker->RegenerateClips({Clip.Get()}));
            Stage = 3;
            return false;
        }
        if (Stage == 3)
        {
            if (Baker->IsBusy())
                return false;
            if (!Test->TestEqual(TEXT("Regenerate succeeds"), Baker->GeneratedClips.Num(), 1))
                return true;
            Test->TestEqual(TEXT("Regenerate preserves identity"), Baker->GeneratedClips[0].Get(), Clip.Get());
            Test->TestTrue(TEXT("Regenerate preserves values"), Clip->Curves == Original);
            Baker->RegenerateClips({Clip.Get()});
            Baker->Cancel();
            Test->TestTrue(TEXT("Cancelled regeneration preserves values"), Clip->Curves == Original);
            ChangedSound.Reset(DuplicateObject<USoundWave>(Clip->SoundWave, GetTransientPackage()));
            Offline.Reset(ULAMOfflineAnalysis::Start(ChangedSound.Get(), {}));
            ChangedSound->CompressedDataGuid = FGuid::NewGuid();
            Stage = 4;
            return false;
        }
        if (Stage == 4)
        {
            if (!Offline->IsDone())
                return false;
            FLAMOfflineResult Result;
            FString Error;
            Test->TestFalse(TEXT("Changed source discarded"), Offline->TakeResult(Result, Error));
            Test->TestTrue(TEXT("Changed source is actionable"), Error.Contains(TEXT("changed")));
            Offline.Reset(ULAMOfflineAnalysis::Start(Clip->SoundWave, {}));
            Offline->Cancel();
            Test->TestFalse(TEXT("Cancelled job publishes no result"), Offline->TakeResult(Result, Error));
            Clip->FormatVersion++;
            Test->TestTrue(TEXT("Old format needs regeneration"), Clip->NeedsRegeneration(Error));
            Clip->RefreshValidation();
            Component->OnPlaybackFailed.AddDynamic(Receiver.Get(), &ULAMTestReceiver::PlaybackFailed);
            Test->TestFalse(TEXT("Invalid baked clip cannot play"), Component->PlayExpressionClip(Clip.Get()));
            Test->TestEqual(TEXT("Existing failure event reports baked corruption"), Receiver->PlaybackErrorCode,
                            FName(TEXT("InvalidBakedClip")));
            Clip->FormatVersion--;
            Clip->RefreshValidation();
            const auto SavedModel = GetDefault<ULAMSettings>()->Model;
            GetMutableDefault<ULAMSettings>()->Model.Reset();
            Offline.Reset(ULAMOfflineAnalysis::Start(Clip->SoundWave, {}));
            Test->TestTrue(TEXT("Missing model completes"), Offline->IsDone());
            Test->TestFalse(TEXT("Missing model has no result"), Offline->TakeResult(Result, Error));
            GetMutableDefault<ULAMSettings>()->Model = SavedModel;
            const FString SourcePath =
                TEXT("/Game/Audio/LAM_BakeCollision_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
            ChangedSound.Reset(DuplicateObject<USoundWave>(Clip->SoundWave, CreatePackage(*SourcePath),
                                                           *FPackageName::GetShortName(SourcePath)));
            const FString CollisionPath = SourcePath + TEXT("_LAMClip");
            Collision.Reset(NewObject<ULAMCurveProfile>(
                CreatePackage(*CollisionPath), *FPackageName::GetShortName(CollisionPath), RF_Public | RF_Standalone));
            Baker->GenerateClips({ChangedSound.Get()}, {});
            Stage = 5;
            return false;
        }
        if (Stage == 5)
        {
            if (Baker->IsBusy())
                return false;
            if (!Test->TestEqual(TEXT("Collision bake succeeds"), Baker->GeneratedClips.Num(), 1))
                return true;
            CollisionClip.Reset(Baker->GeneratedClips[0]);
            Test->TestEqual(TEXT("Unrelated collision gets suffix"), CollisionClip->GetOutermost()->GetName(),
                            Collision->GetOutermost()->GetName() + TEXT("_1"));
            Baker->GenerateClips({ChangedSound.Get()}, {});
            Stage = 6;
            return false;
        }
        if (Stage == 6)
        {
            if (Baker->IsBusy())
                return false;
            if (!Test->TestEqual(TEXT("Repeated generate succeeds"), Baker->GeneratedClips.Num(), 1))
                return true;
            Test->TestEqual(TEXT("Repeated generate updates existing suffix"), Baker->GeneratedClips[0].Get(),
                            CollisionClip.Get());
            Clip->Settings.Style = 12;
            Baker->RegenerateClips({Clip.Get()});
            Stage = 7;
            return false;
        }
        if (Stage == 7)
        {
            if (Baker->IsBusy())
                return false;
            Clip->Settings.Style = 0;
            Test->TestEqual(TEXT("Failed regeneration reported"), Baker->Errors.Num(), 1);
            Test->TestTrue(TEXT("Failed regeneration preserves data"), Clip->Curves == Original);
            Baker->RegenerateClips({Clip.Get()});
            Stage = 8;
            return false;
        }
        if (Stage == 8)
        {
            Baker->Cancel();
            Test->TestTrue(TEXT("Running cancellation preserves data"), Clip->Curves == Original);
            return true;
        }
        return false;
    }
};
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMBakeIntegrationTest, "LAM.Bake.GenerateAndRegenerate",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMBakeIntegrationTest::RunTest(const FString &)
{
    ADD_LATENT_AUTOMATION_COMMAND(FLAMBakeIntegration(this));
    return true;
}
#endif
