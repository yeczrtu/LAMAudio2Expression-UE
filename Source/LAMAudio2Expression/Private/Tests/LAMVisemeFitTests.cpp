#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LAMViseme.h"
#include "HAL/PlatformTime.h"
#include <limits>
#include "LAMVisemeReference.inl"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMVisemeFitTest, "LAM.Viseme.TemplateFit",
                               EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMVisemeFitTest::RunTest(const FString &)
{
    double MaxGap = 0;
    for (const auto &Case : VisemeReferenceCases)
    {
        FLAMVisemeSettings Settings;
        Settings.ConversionMode = ELAMVisemeConversionMode::TemplateFit;
        Settings.Template = static_cast<ELAMVisemeTemplate>(Case.Template);
        FLAMExpressionFrame Frame;
        Frame.bValid = true;
        Frame.Weight = .37f;
        Frame.TimeSeconds = 1.25f;
        Frame.Values.Append(Case.Input, 52);
        for (int32 I = 0; I < 52; ++I)
        {
            FLAMVisemeInputCorrection C;
            C.SourceName = LAM::CurveNames()[I];
            C.FitWeight = Case.Q[I];
            Settings.InputCorrections.Add(C);
        }
        const auto Prepared = LAM::PrepareVisemeSettings(Settings);
        const auto Result = LAM::ConvertARKitToVisemes(Frame, Prepared);
        TestTrue(TEXT("Valid fitted frame"), Result.bValid);
        TestEqual(TEXT("Weight is metadata"), Result.Weight, Frame.Weight);
        TestEqual(TEXT("Time preserved"), Result.TimeSeconds, Frame.TimeSeconds);
        const auto &Basis = LAM::VisemeBasis(Settings.Template);
        double Objective = 0, Total = 0;
        for (int32 J = 1; J < 15; ++J)
        {
            const double Value = Result.Values[J];
            TestTrue(TEXT("Finite bounded weight"), FMath::IsFinite(Value) && Value >= 0 && Value <= 1);
            Objective += 1.e-4 * Value * Value;
            Total += Value;
        }
        for (int32 I = 0; I < 52; ++I)
        {
            double Predicted = 0;
            for (int32 J = 0; J < 14; ++J)
                Predicted += Basis.Values[I][J] * Result.Values[J + 1];
            float Input = FMath::Clamp(Case.Input[I], 0.f, 1.f);
            if (I == 24)
                Input *= 1.f - FMath::Clamp(Case.Input[26], 0.f, 1.f);
            Objective += Case.Q[I] * FMath::Square(Predicted - Input);
        }
        MaxGap = FMath::Max(MaxGap, FMath::Abs(Objective - Case.Objective));
        TestTrue(TEXT("Objective agrees with independent SLSQP within 1e-5"),
                 FMath::Abs(Objective - Case.Objective) <= 1.e-5);
        TestTrue(TEXT("Non-neutral sum <= 1"), Total <= 1.000001);
        TestTrue(TEXT("sil residual"), FMath::Abs(Result.Values[0] + Total - 1) < 1.e-6);
        if (Settings.Template == ELAMVisemeTemplate::TalkingHead)
            TestEqual(TEXT("Indistinguishable CH/RR split equally"), Result.Values[6], Result.Values[9]);
        auto Perturbed = Frame;
        Perturbed.Values[24] += 1.e-5f;
        const auto Nearby = LAM::ConvertARKitToVisemes(Perturbed, Prepared);
        for (int32 J = 0; J < 15; ++J)
            TestTrue(TEXT("Small input change stays continuous"),
                     FMath::Abs(Result.Values[J] - Nearby.Values[J]) < .01f);
    }
    AddInfo(FString::Printf(TEXT("174 independent reference cases; max objective gap %.9g"), MaxGap));
    for (const auto Template : {ELAMVisemeTemplate::OpenFaceFX, ELAMVisemeTemplate::TalkingHead})
    {
        auto *Profile = NewObject<ULAMVisemeProfile>();
        Profile->Settings.ConversionMode = ELAMVisemeConversionMode::TemplateFit;
        Profile->Settings.Template = Template;
        Profile->ApplyNamePreset(ELAMVisemePreset::OculusPrefixed);
        TestEqual(TEXT("Prefixed I alias"), Profile->TargetNames[ELAMViseme::ih], FName(TEXT("viseme_I")));
        TestEqual(TEXT("Prefixed sil"), Profile->TargetNames[ELAMViseme::sil], FName(TEXT("viseme_sil")));
        TestTrue(TEXT("Name preset keeps mode"), Profile->Settings.ConversionMode == ELAMVisemeConversionMode::TemplateFit);
        const auto &B = LAM::VisemeBasis(Template);
        FLAMExpressionFrame Frame;
        Frame.bValid = true;
        Frame.Weight = 1;
        Frame.Values.Init(0, 52);
        TestEqual(TEXT("Neutral sil"), ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile).Values[0], 1.f);
        for (int32 J = 0; J < 14; ++J)
        {
            for (int32 I = 0; I < 52; ++I)
                Frame.Values[I] = float(B.Values[I][J]);
            const auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile);
            if (Template == ELAMVisemeTemplate::TalkingHead && (J == 5 || J == 8))
                TestTrue(TEXT("CH/RR recover combined activation"), R.Values[6] + R.Values[9] > .95f);
            else
            {
                TestTrue(TEXT("Template's own slot activates"), R.Values[J + 1] > .75f);
                for (int32 K = 1; K < 15; ++K)
                    if (K != J + 1)
                        TestTrue(TEXT("Template's own slot dominates"), R.Values[J + 1] > R.Values[K]);
            }
        }
        Frame.Values.Init(0, 52);
        for (int32 I = 0; I < 52; ++I)
            Frame.Values[I] = float(B.Values[I][0]); // PP: no jaw gate.
        TestTrue(TEXT("PP with jaw closed"), ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile).Values[1] > .99f);
        Profile->Settings.VisemeGains.Add(ELAMViseme::PP, 0);
        TestEqual(TEXT("Per-viseme gain"), ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile).Values[1], 0.f);
        Profile->Settings.VisemeGains.Reset();
        for (int32 J = 1; J < 15; ++J)
            Profile->Settings.VisemeGains.Add(static_cast<ELAMViseme>(J), std::numeric_limits<float>::max());
        auto R = ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile);
        double Sum = 0;
        for (float Value : R.Values)
        {
            TestTrue(TEXT("Extreme gains remain finite"), FMath::IsFinite(Value));
            Sum += Value;
        }
        TestTrue(TEXT("Extreme gains normalize"), FMath::Abs(Sum - 1) < 1.e-6);
        Profile->Settings.VisemeGains.Reset();
        auto Invalid = Frame;
        Invalid.Values[0] = std::numeric_limits<float>::quiet_NaN();
        TestFalse(TEXT("Even unused nonfinite inputs rejected"), ULAMVisemeLibrary::ConvertARKitToVisemes(Invalid, Profile).bValid);
        Invalid = Frame;
        Invalid.Values.Pop();
        TestFalse(TEXT("Wrong length rejected"), ULAMVisemeLibrary::ConvertARKitToVisemes(Invalid, Profile).bValid);
        Invalid = Frame;
        Invalid.bValid = false;
        TestFalse(TEXT("Invalid input rejected"), ULAMVisemeLibrary::ConvertARKitToVisemes(Invalid, Profile).bValid);
        for (int32 I = 0; I < 52; ++I)
        {
            FLAMVisemeInputCorrection C;
            C.SourceName = LAM::CurveNames()[I];
            C.FitWeight = 0;
            Profile->Settings.InputCorrections.Add(C);
        }
        TestFalse(TEXT("No observed template channels rejected"), ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile).bValid);
        Profile->Settings.InputCorrections.Reset();
        FLAMVisemeInputCorrection Correction;
        Correction.SourceName = TEXT("tongueOut");
        Correction.FitWeight = 2;
        Profile->Settings.InputCorrections.Add(Correction);
        TestFalse(TEXT("Invalid confidence rejected"), ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile).bValid);
        Profile->Settings.InputCorrections.Reset();
        const auto Prepared = LAM::PrepareVisemeSettings(Profile->Settings);
        TArray<double> Times;
        FRandomStream Random(52);
        for (int32 Sample = 0; Sample < 256; ++Sample)
        {
            for (float &Value : Frame.Values)
                Value = Random.FRand();
            const double Started = FPlatformTime::Seconds();
            R = LAM::ConvertARKitToVisemes(Frame, Prepared);
            Times.Add((FPlatformTime::Seconds() - Started) * 1.e6);
        }
        Times.Sort();
        AddInfo(FString::Printf(TEXT("Template %d prepared conversion: median %.2f us, p95 %.2f us (256 random frames)"),
                               int32(Template), Times[128], Times[243]));
    }
    return true;
}
#endif
