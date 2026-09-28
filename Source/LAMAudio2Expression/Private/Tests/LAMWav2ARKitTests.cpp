#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LAMCore.h"
#include "LAMSettings.h"
#include "Misc/CommandLine.h"
#include "Misc/FileHelper.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMWav2ARKitTest, "LAM.Wav2ARKit.CPU",
                               EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMWav2ARKitTest::RunTest(const FString&)
{
    if (!FParse::Param(FCommandLine::Get(), TEXT("LAMWav2ARKitTests")))
    {
        AddInfo(TEXT("Optional model test: enable with -LAMWav2ARKitTests after importing its fixtures."));
        return true;
    }
    auto* Model = LoadObject<UNNEModelData>(nullptr, TEXT("/LAMAudio2Expression/Models/Wav2ARKit_CPU.Wav2ARKit_CPU"));
    if (!TestNotNull(TEXT("Imported Wav2ARKit"), Model)) return false;
    const auto Models = LAM::CreateModels(Model, true);
    TestTrue(TEXT("CPU model available"), Models.CPU.IsValid());
    TestFalse(TEXT("CPU target excludes GPU even when preferred"), Models.GPU.IsValid());
    FString Directory = FPaths::ProjectDir() / TEXT(".work/wav2arkit_cpu/fixtures"), Report;
    FParse::Value(FCommandLine::Get(), TEXT("LAMWav2ARKitFixtureDir="), Directory);
    for (const FString Name : {TEXT("silence"), TEXT("noise"), TEXT("speech")})
    {
        TArray<uint8> Input, Reference;
        FFileHelper::LoadFileToArray(Input, *(Directory / (Name + TEXT(".audio.f32"))));
        FFileHelper::LoadFileToArray(Reference, *(Directory / (Name + TEXT(".expected.f32"))));
        if (!TestEqual(TEXT("Input fixture size"), Input.Num(), LAM::Window * 4) ||
            !TestEqual(TEXT("Reference fixture size"), Reference.Num(), LAM::WindowFrames * 52 * 4)) return false;
        TArray<float> Audio, Output, First;
        Audio.SetNumUninitialized(LAM::Window);
        FMemory::Memcpy(Audio.GetData(), Input.GetData(), Input.Num());
        TSharedPtr<UE::NNE::IModelInstanceRunSync> Instance;
        bool GPU = false;
        for (int32 Style : {0, 11})
        {
            float InitMs = 0;
            const double Start = FPlatformTime::Seconds();
            if (!TestTrue(TEXT("First/repeated inference"), LAM::InferWindow(Audio, Style, Models, Instance, GPU, Output, &InitMs)))
                return false;
            const double ElapsedMs = (FPlatformTime::Seconds() - Start) * 1000;
            float MaxError = 0;
            for (int32 I = 0; I < Output.Num(); ++I)
            {
                float Expected;
                FMemory::Memcpy(&Expected, Reference.GetData() + I * 4, 4);
                MaxError = FMath::Max(MaxError, FMath::Abs(Expected - Output[I]));
            }
            TestTrue(TEXT("Matches Python ORT within 1e-3"), MaxError <= 1.e-3f);
            if (Style == 0) First = Output;
            else TestTrue(TEXT("Style is ignored; repeated result identical"), First == Output);
            const FString Line = FString::Printf(TEXT("%s style=%d max_error=%.9f initialization_ms=%.2f inference_ms=%.2f\n"),
                *Name, Style, MaxError, InitMs, ElapsedMs - InitMs);
            AddInfo(Line); Report += Line;
        }
        Audio.Pop();
        TestFalse(TEXT("Wrong input length rejected before running"), LAM::InferWindow(Audio, 0, Models, Instance, GPU, Output));
    }
    for (int32 Samples : {1, 16000, 16001, 48001})
    {
        TArray<float> Audio;
        Audio.Init(.01f, Samples);
        FLAMJob Job;
        LAM::Analyze(Audio, Models, {}, Job);
        TestTrue(TEXT("Boundary analysis succeeds"), Job.Error.IsEmpty());
        TestEqual(TEXT("Exact frame count"), Job.Curves.Num(), int32((int64(Samples) * 30 + 15999) / 16000) * 52);
    }
    for (const FString Name : {TEXT("bad_name"), TEXT("bad_type"), TEXT("bad_shape"), TEXT("bad_dynamic"), TEXT("nonfinite")})
    {
        auto* Invalid = LoadObject<UNNEModelData>(nullptr, *(TEXT("/Game/Wav2ARKitTests/Models/") + Name + TEXT(".") + Name));
        if (!TestNotNull(TEXT("Invalid-model fixture"), Invalid)) return false;
        const auto InvalidModels = LAM::CreateModels(Invalid, false);
        if (!TestTrue(TEXT("Fixture model loads"), InvalidModels.CPU.IsValid())) return false;
        TArray<float> Audio, Output;
        Audio.Init(0, LAM::Window);
        TSharedPtr<UE::NNE::IModelInstanceRunSync> Instance;
        bool GPU = false;
        TestFalse(*Name, LAM::InferWindow(Audio, 0, InvalidModels, Instance, GPU, Output));
        FLAMJob Job;
        LAM::Analyze(Audio, InvalidModels, {}, Job);
        TestFalse(TEXT("Invalid result reports failure"), Job.Error.IsEmpty());
        TestTrue(TEXT("Invalid result not published"), Job.Curves.IsEmpty());
    }
    FFileHelper::SaveStringToFile(Report, *(FPaths::ProjectSavedDir() / TEXT("Wav2ARKitParity.txt")));
    return true;
}
#endif
