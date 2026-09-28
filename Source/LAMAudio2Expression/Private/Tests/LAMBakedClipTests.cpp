#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LAMBakedExpressionClip.h"
#include "LAMOfflineAnalysis.h"
#include "Sound/SoundWave.h"
#include "UObject/StrongObjectPtr.h"
#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMBakedClipValidation, "LAM.Bake.DataValidation",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMBakedClipValidation::RunTest(const FString &)
{
    TStrongObjectPtr<ULAMBakedExpressionClip> Clip(NewObject<ULAMBakedExpressionClip>());
    Clip->SoundWave = NewObject<USoundWave>();
    Clip->FormatVersion = ULAMBakedExpressionClip::CurrentFormatVersion;
    FString Error;
    for (int32 Samples : {1, 399, 16000, 16001, 34192, 4800000})
    {
        Clip->NumSamples = Samples;
        Clip->Duration = float(Samples) / LAM::Rate;
        Clip->Curves.Init(.25f, ((int64(Samples) * 30 + 15999) / 16000) * 52);
        Clip->RefreshValidation();
        TestTrue(TEXT("Sample boundaries are valid"), Clip->GetPlaybackValidationError().IsEmpty());
        TestEqual(TEXT("End clamp"), Clip->Sample(400).Values[24], .25f);
    }
    Clip->Curves[0] = std::numeric_limits<float>::quiet_NaN();
    TestFalse(TEXT("Non-finite payload rejected"), Clip->ValidateBakedData(Error));
    Clip->Curves[0] = .25f;
    Clip->FormatVersion++;
    TestFalse(TEXT("Unsupported format rejected"), Clip->ValidateBakedData(Error));
    Clip->FormatVersion--;
    Clip->Curves.Pop();
    TestFalse(TEXT("Truncated payload rejected"), Clip->ValidateBakedData(Error));
    TStrongObjectPtr<ULAMOfflineAnalysis> Job(ULAMOfflineAnalysis::Start(nullptr, {}));
    FLAMOfflineResult Result;
    TestTrue(TEXT("Invalid input completes without a world"), Job->IsDone());
    TestFalse(TEXT("Invalid input has no result"), Job->TakeResult(Result, Error));
    TestFalse(TEXT("Actionable error"), Error.IsEmpty());
    return true;
}
#endif
