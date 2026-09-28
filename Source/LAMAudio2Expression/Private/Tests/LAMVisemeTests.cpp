#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LAMViseme.h"
#include <limits>

namespace
{
FLAMExpressionFrame Input(float Jaw = 0, float Stretch = 0, float Round = 0)
{
    FLAMExpressionFrame F;
    F.bValid = true;
    F.Weight = 1;
    F.Values.Init(0.f, LAM::CurveCount);
    auto Set = [&](const TCHAR *Name, float Value) { F.Values[LAM::CurveNames().IndexOfByKey(Name)] = Value; };
    Set(TEXT("jawOpen"), Jaw);
    Set(TEXT("mouthStretchLeft"), Stretch);
    Set(TEXT("mouthStretchRight"), Stretch);
    Set(TEXT("mouthFunnel"), Round);
    return F;
}
void Set(FLAMExpressionFrame &F, const TCHAR *Name, float V)
{
    F.Values[LAM::CurveNames().IndexOfByKey(Name)] = V;
}
} // namespace

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMVisemeRulesTest, "LAM.Viseme.RulesAndPresets",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMVisemeRulesTest::RunTest(const FString &)
{
    auto *Profile = NewObject<ULAMVisemeProfile>();
    TArray<FString> Errors;
    TestTrue(TEXT("Default profile valid"), Profile->ValidateProfile(Errors));
    const auto Names = ULAMVisemeLibrary::GetVisemeNames();
    TestEqual(TEXT("15 slots"), Names.Num(), 15);
    const TCHAR *Expected[] = {TEXT("sil"), TEXT("PP"), TEXT("FF"), TEXT("TH"), TEXT("DD"),
                               TEXT("kk"),  TEXT("CH"), TEXT("SS"), TEXT("nn"), TEXT("RR"),
                               TEXT("aa"),  TEXT("E"),  TEXT("ih"), TEXT("oh"), TEXT("ou")};
    for (int32 I = 0; I < 15; ++I)
        TestEqual(TEXT("SDK order"), Names[I], FName(Expected[I]));
    const TArray<FLAMExpressionFrame> Cases = {Input(.8f), Input(.15f, .8f), Input(.15f, 0, .8f), Input(.65f, .8f),
                                               Input(.65f, 0, .8f)};
    const int32 Slots[] = {10, 12, 14, 11, 13};
    for (int32 C = 0; C < Cases.Num(); ++C)
    {
        const auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(Cases[C], Profile);
        TestTrue(TEXT("Representative input valid"), R.bValid);
        TestTrue(TEXT("Expected vowel is active"), R.Values[Slots[C]] > .4f);
        for (const int32 Other : Slots)
            if (Other != Slots[C])
                TestTrue(TEXT("Expected vowel dominates"), R.Values[Slots[C]] > R.Values[Other]);
        const auto W = ULAMVisemeLibrary::GetVowelWeights(R);
        const float Five[] = {W.A, W.I, W.U, W.E, W.O};
        for (int32 I = 0; I < 5; ++I)
            TestEqual(TEXT("A I U E O helper order"), Five[I], R.Values[Slots[I]]);
    }
    auto Neutral = Input();
    for (int32 Case = 0; Case < 4; ++Case)
    {
        if (Case == 1)
        {
            Set(Neutral, TEXT("mouthSmileLeft"), 1);
            Set(Neutral, TEXT("mouthSmileRight"), 1);
        }
        if (Case == 2)
            Set(Neutral, TEXT("mouthPucker"), 1);
        if (Case == 3)
        {
            Set(Neutral, TEXT("jawOpen"), 1);
            Set(Neutral, TEXT("mouthClose"), 1);
        }
        const auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(Neutral, nullptr);
        TestEqual(TEXT("Closed mouth neutral"), R.Values[0], 1.f);
        for (int32 I = 1; I < 15; ++I)
            TestEqual(TEXT("No vowel from closed expression"), R.Values[I], 0.f);
    }
    auto F = Cases[0];
    F.Weight = .3f;
    F.TimeSeconds = 2.5f;
    auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(F, Profile);
    TestEqual(TEXT("Weight separate from vowel"), R.Values[10], 1.f);
    TestEqual(TEXT("Weight preserved"), R.Weight, .3f);
    TestEqual(TEXT("Time preserved"), R.TimeSeconds, 2.5f);
    FLAMVisemeInputCorrection Correction;
    Correction.SourceName = TEXT("jawOpen");
    Correction.Baseline = .8f;
    Profile->Settings.InputCorrections.Add(Correction);
    TestEqual(TEXT("Neutral calibration suppresses jaw baseline"),
              ULAMVisemeLibrary::ConvertARKitToVisemes(F, Profile).Values[0], 1.f);
    Profile->Settings.InputCorrections.Reset();
    F = Input();
    Set(F, TEXT("mouthUpperUpLeft"), 1);
    Set(F, TEXT("mouthUpperUpRight"), 1);
    TestTrue(TEXT("Lip opening without jaw can drive vowel"),
             ULAMVisemeLibrary::ConvertARKitToVisemes(F, Profile).Values[10] > .9f);
    F = Input(.15f);
    Set(F, TEXT("mouthPucker"), 1);
    TestTrue(TEXT("Open pucker drives U"),
             ULAMVisemeLibrary::GetVowelWeights(ULAMVisemeLibrary::ConvertARKitToVisemes(F, Profile)).U > .4f);
    for (auto Preset : {ELAMVisemePreset::JapaneseFive, ELAMVisemePreset::OculusReference, ELAMVisemePreset::OculusSDK})
    {
        Profile->ApplyNamePreset(Preset);
        TestTrue(TEXT("Preset valid"), Profile->ValidateProfile(Errors));
        TestEqual(TEXT("Preset count"), Profile->TargetNames.Num(), Preset == ELAMVisemePreset::JapaneseFive ? 5 : 15);
        TestEqual(TEXT("I alias"), Profile->TargetNames[ELAMViseme::ih],
                  FName(Preset == ELAMVisemePreset::OculusSDK ? TEXT("ih") : TEXT("I")));
        TestEqual(TEXT("O alias"), Profile->TargetNames[ELAMViseme::oh],
                  FName(Preset == ELAMVisemePreset::OculusSDK ? TEXT("oh") : TEXT("O")));
        TestEqual(TEXT("U alias"), Profile->TargetNames[ELAMViseme::ou],
                  FName(Preset == ELAMVisemePreset::OculusSDK ? TEXT("ou") : TEXT("U")));
        TestEqual(TEXT("A alias"), Profile->TargetNames[ELAMViseme::aa],
                  FName(Preset == ELAMVisemePreset::JapaneseFive ? TEXT("A") : TEXT("aa")));
    }
    auto *Upper = NewObject<ULAMCurveProfile>();
    ULAMVisemeLibrary::ApplyUpperFaceOnlyPreset(Upper);
    TestEqual(TEXT("All 28 jaw/mouth/tongue rules disabled"), Upper->Rules.Num(), 28);
    for (const auto &Rule : Upper->Rules)
        TestFalse(TEXT("Upper-face mask disabled"), Rule.bEnabled);
    TestFalse(TEXT("Eye remains enabled"),
              Upper->Rules.ContainsByPredicate([](const FLAMCurveRule &Rule)
                                               { return Rule.SourceName == TEXT("eyeBlinkLeft"); }));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMVisemeBoundsTest, "LAM.Viseme.BoundsContinuityAndInvalidInput",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMVisemeBoundsTest::RunTest(const FString &)
{
    auto *P = NewObject<ULAMVisemeProfile>();
    P->Settings.VowelGains.A = 4;
    P->Settings.VowelGains.U = 7;
    FRandomStream Random(1234);
    for (int32 Iteration = 0; Iteration < 2048; ++Iteration)
    {
        auto F = Input();
        for (float &V : F.Values)
            V = Random.FRandRange(-.5f, 1.5f);
        const auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(F, P);
        TestTrue(TEXT("Random finite input valid"), R.bValid);
        float Sum = 0;
        for (int32 I = 0; I < 15; ++I)
        {
            TestTrue(TEXT("Finite normalized output"),
                     FMath::IsFinite(R.Values[I]) && R.Values[I] >= 0 && R.Values[I] <= 1);
            Sum += R.Values[I];
            if (I >= 1 && I <= 9)
                TestEqual(TEXT("Consonant zero"), R.Values[I], 0.f);
        }
        TestTrue(TEXT("Neutral + vowels total one"), FMath::Abs(Sum - 1) < 1.e-6f);
    }
    P->Settings = FLAMVisemeSettings();
    // Dense input sweeps cross every threshold and both vowel splits, including mixed shapes.
    for (int32 Axis = 0; Axis < 3; ++Axis)
    {
        FLAMVisemeFrame Previous;
        for (int32 Step = 0; Step <= 1000; ++Step)
        {
            const float T = Step / 1000.f;
            const auto F = Input(Axis == 0 ? T : .35f, Axis == 1 ? T : .35f, Axis == 2 ? T : .35f);
            const auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(F, P);
            if (Step)
                for (int32 I = 0; I < 15; ++I)
                    TestTrue(TEXT("No discontinuous switch"), FMath::Abs(R.Values[I] - Previous.Values[I]) < .02f);
            Previous = R;
        }
    }
    const float NaN = std::numeric_limits<float>::quiet_NaN(), Inf = std::numeric_limits<float>::infinity();
    for (int32 Bad = 0; Bad < 6; ++Bad)
    {
        auto F = Input(.8f);
        if (Bad == 0)
            F.bValid = false;
        if (Bad == 1)
            F.Values.Pop();
        if (Bad == 2)
            F.Values.Add(0);
        if (Bad == 3)
            F.Values[0] = NaN;
        if (Bad == 4)
            F.Weight = Inf;
        if (Bad == 5)
            F.TimeSeconds = NaN;
        const auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(F, P);
        TestFalse(TEXT("Invalid frame rejected"), R.bValid);
        TestEqual(TEXT("Invalid weight zero"), R.Weight, 0.f);
        for (float V : R.Values)
            TestEqual(TEXT("Invalid output zero including sil"), V, 0.f);
    }
    TArray<FString> Errors;
    P->Settings.Width.High = P->Settings.Width.Low;
    TestFalse(TEXT("Empty smoothstep range rejected"), P->ValidateProfile(Errors));
    TestFalse(TEXT("Invalid profile not evaluated"), ULAMVisemeLibrary::ConvertARKitToVisemes(Input(), P).bValid);
    P->Settings = FLAMVisemeSettings();
    P->Settings.VowelGains.I = NaN;
    TestFalse(TEXT("NaN gain rejected"), P->ValidateProfile(Errors));
    P->Settings = FLAMVisemeSettings();
    P->TargetNames[ELAMViseme::ih] = TEXT("A");
    TestFalse(TEXT("Duplicate destination rejected"), P->ValidateProfile(Errors));
    P->ApplyNamePreset(ELAMVisemePreset::JapaneseFive);
    P->TargetNames[ELAMViseme::ih] = NAME_None;
    TestTrue(TEXT("Unassigned destination allowed"), P->ValidateProfile(Errors));
    FLAMVisemeInputCorrection C;
    C.SourceName = TEXT("notARKit");
    P->Settings.InputCorrections.Add(C);
    TestFalse(TEXT("Unknown correction rejected"), P->ValidateProfile(Errors));
    P->Settings.InputCorrections[0].SourceName = TEXT("jawOpen");
    const auto Duplicate = P->Settings.InputCorrections[0];
    P->Settings.InputCorrections.Add(Duplicate);
    TestFalse(TEXT("Duplicate correction rejected"), P->ValidateProfile(Errors));
    P->Settings = FLAMVisemeSettings();
    P->Settings.VowelGains.A = std::numeric_limits<float>::max();
    TestEqual(TEXT("Large finite gain remains bounded"),
              ULAMVisemeLibrary::ConvertARKitToVisemes(Input(1), P).Values[10], 1.f);
    return true;
}
#endif
