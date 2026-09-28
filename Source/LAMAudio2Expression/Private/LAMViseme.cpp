#include "LAMViseme.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

namespace LAM
{
FLAMVisemeFrame FitLAMVisemeTemplate(const FLAMExpressionFrame &, const float *, const FPreparedVisemeSettings &);
}
namespace
{
bool NonNegative(float V)
{
    return FMath::IsFinite(V) && V >= 0;
}
float Smooth(const FLAMVisemeRange &Range, float V)
{
    const float T = FMath::Clamp((V - Range.Low) / (Range.High - Range.Low), 0.f, 1.f);
    return T * T * (3.f - 2.f * T);
}
} // namespace

FLAMVisemeSettings::FLAMVisemeSettings()
{
    for (int32 I = 1; I < LAM::VisemeCount; ++I)
        VisemeGains.Add(static_cast<ELAMViseme>(I), 1.f);
    VowelGains.A = VowelGains.I = VowelGains.U = VowelGains.E = VowelGains.O = 1.f;
    Activation.Low = .05f;
    Activation.High = .25f;
    OpenSplit.Low = .20f;
    OpenSplit.High = .50f;
}
ULAMVisemeProfile::ULAMVisemeProfile()
{
    ApplyNamePreset(ELAMVisemePreset::JapaneseFive);
}
TArray<FName> ULAMVisemeLibrary::GetVisemeNames()
{
    return {TEXT("sil"), TEXT("PP"), TEXT("FF"), TEXT("TH"), TEXT("DD"), TEXT("kk"), TEXT("CH"), TEXT("SS"),
            TEXT("nn"),  TEXT("RR"), TEXT("aa"), TEXT("E"),  TEXT("ih"), TEXT("oh"), TEXT("ou")};
}
void ULAMVisemeProfile::ApplyNamePreset(ELAMVisemePreset Preset)
{
    TargetNames.Reset();
    if (Preset == ELAMVisemePreset::JapaneseFive)
    {
        TargetNames.Add(ELAMViseme::aa, TEXT("A"));
        TargetNames.Add(ELAMViseme::ih, TEXT("I"));
        TargetNames.Add(ELAMViseme::ou, TEXT("U"));
        TargetNames.Add(ELAMViseme::E, TEXT("E"));
        TargetNames.Add(ELAMViseme::oh, TEXT("O"));
        return;
    }
    const auto Names = ULAMVisemeLibrary::GetVisemeNames();
    for (int32 I = 0; I < LAM::VisemeCount; ++I)
        TargetNames.Add(static_cast<ELAMViseme>(I), Names[I]);
    if (Preset == ELAMVisemePreset::OculusReference || Preset == ELAMVisemePreset::OculusPrefixed)
    {
        TargetNames[ELAMViseme::ih] = TEXT("I");
        TargetNames[ELAMViseme::oh] = TEXT("O");
        TargetNames[ELAMViseme::ou] = TEXT("U");
    }
    if (Preset == ELAMVisemePreset::OculusPrefixed)
        for (auto &Pair : TargetNames)
            Pair.Value = FName(*(TEXT("viseme_") + Pair.Value.ToString()));
}
bool LAM::ValidateVisemeSettings(const FLAMVisemeSettings &S, TArray<FString> &Errors)
{
    const int32 Before = Errors.Num();
    if (S.ConversionMode != ELAMVisemeConversionMode::FiveVowelRules &&
        S.ConversionMode != ELAMVisemeConversionMode::TemplateFit)
        Errors.Add(TEXT("Unknown viseme conversion mode."));
    if (S.ConversionMode == ELAMVisemeConversionMode::FiveVowelRules)
    {
        for (const auto &Pair : {TPair<const TCHAR *, FLAMVisemeRange>(TEXT("Width"), S.Width),
                                 {TEXT("Roundness"), S.Roundness},
                                 {TEXT("Activation"), S.Activation},
                                 {TEXT("OpenSplit"), S.OpenSplit}})
            if (!NonNegative(Pair.Value.Low) || !FMath::IsFinite(Pair.Value.High) || Pair.Value.High > 1 ||
                Pair.Value.High - Pair.Value.Low < 1.e-6f)
                Errors.Add(FString::Printf(TEXT("%s requires finite 0 <= Low < High <= 1 (gap >= 1e-6)."), Pair.Key));
        if (!NonNegative(S.LipOpenContribution) || !NonNegative(S.SmileContribution))
            Errors.Add(TEXT("LipOpenContribution and SmileContribution must be finite and nonnegative."));
        for (float V : {S.VowelGains.A, S.VowelGains.I, S.VowelGains.U, S.VowelGains.E, S.VowelGains.O})
            if (!NonNegative(V))
            {
                Errors.Add(TEXT("Vowel gains must be finite and nonnegative."));
                break;
            }
    }
    if (S.ConversionMode == ELAMVisemeConversionMode::TemplateFit)
    {
        if (S.Template != ELAMVisemeTemplate::OpenFaceFX && S.Template != ELAMVisemeTemplate::TalkingHead)
            Errors.Add(TEXT("Unknown viseme template."));
        for (const auto &Gain : S.VisemeGains)
            if (static_cast<uint8>(Gain.Key) == 0 || static_cast<uint8>(Gain.Key) >= LAM::VisemeCount ||
                !NonNegative(Gain.Value))
                Errors.Add(TEXT("Template gains require non-neutral slots and finite nonnegative values."));
    }
    TSet<FName> Seen;
    for (const auto &C : S.InputCorrections)
    {
        if (!CurveNames().Contains(C.SourceName) || Seen.Contains(C.SourceName))
            Errors.Add(FString::Printf(TEXT("Unknown or duplicate ARKit input: %s"), *C.SourceName.ToString()));
        Seen.Add(C.SourceName);
        if (S.ConversionMode == ELAMVisemeConversionMode::TemplateFit &&
            (!NonNegative(C.FitWeight) || C.FitWeight > 1))
            Errors.Add(TEXT("FitWeight must be finite and in [0,1]."));
        if (!NonNegative(C.Baseline) || C.Baseline > 1 || !NonNegative(C.Scale))
            Errors.Add(TEXT("Input corrections require finite baseline in [0,1] and nonnegative scale."));
    }
    if (S.ConversionMode == ELAMVisemeConversionMode::TemplateFit && Before == Errors.Num())
    {
        const auto &B = LAM::VisemeBasis(S.Template);
        bool bObserved = false;
        for (int32 I = 0; I < LAM::CurveCount; ++I)
        {
            const auto *Correction = S.InputCorrections.FindByPredicate(
                [&](const auto &C) { return C.SourceName == LAM::CurveNames()[I]; });
            if (!Correction || Correction->FitWeight > 0)
                for (double Value : B.Values[I])
                    bObserved |= Value != 0;
        }
        if (!bObserved)
            Errors.Add(TEXT("Template fitting requires at least one observed template channel."));
    }
    return Before == Errors.Num();
}
bool ULAMVisemeProfile::ValidateProfile(TArray<FString> &Errors) const
{
    Errors.Reset();
    LAM::ValidateVisemeSettings(Settings, Errors);
    TSet<FName> Seen;
    for (const auto &Binding : TargetNames)
    {
        if (static_cast<uint8>(Binding.Key) >= LAM::VisemeCount)
            Errors.Add(TEXT("Invalid viseme slot."));
        if (Binding.Value.IsNone())
            continue;
        if (Seen.Contains(Binding.Value))
            Errors.Add(FString::Printf(TEXT("Duplicate target curve: %s"), *Binding.Value.ToString()));
        Seen.Add(Binding.Value);
    }
    return Errors.IsEmpty();
}
#if WITH_EDITOR
EDataValidationResult ULAMVisemeProfile::IsDataValid(FDataValidationContext &Context) const
{
    const auto SuperResult = Super::IsDataValid(Context);
    TArray<FString> Errors;
    ValidateProfile(Errors);
    for (const auto &Error : Errors)
        Context.AddError(FText::FromString(Error));
    return Errors.IsEmpty() && SuperResult != EDataValidationResult::Invalid ? EDataValidationResult::Valid
                                                                             : EDataValidationResult::Invalid;
}
#endif
FLAMVisemeFrame LAM::ConvertARKitToVisemes(const FLAMExpressionFrame &Frame, const FLAMVisemeSettings &S)
{
    return ConvertARKitToVisemes(Frame, PrepareVisemeSettings(S));
}
FLAMVisemeFrame LAM::ConvertARKitToVisemes(const FLAMExpressionFrame &Frame, const FPreparedVisemeSettings &Prepared)
{
    const auto &S = Prepared.Settings;
    FLAMVisemeFrame Result;
    if (!Frame.bValid || Frame.Values.Num() != CurveCount || !FMath::IsFinite(Frame.TimeSeconds) ||
        !FMath::IsFinite(Frame.Weight))
        return Result;
    if (!Prepared.bValid)
        return Result;
    float V[CurveCount];
    for (int32 I = 0; I < CurveCount; ++I)
    {
        if (!FMath::IsFinite(Frame.Values[I]))
            return Result;
        V[I] = FMath::Clamp(Frame.Values[I], 0.f, 1.f);
    }
    for (const auto &C : S.InputCorrections)
    {
        const int32 I = CurveNames().IndexOfByKey(C.SourceName);
        V[I] = FMath::Clamp((V[I] - C.Baseline) * C.Scale, 0.f, 1.f);
    }
    if (S.ConversionMode == ELAMVisemeConversionMode::TemplateFit)
    {
        // The solver is value-only and consumes already corrected ARKit observations.
        V[24] *= 1.f - V[26];
        return FitLAMVisemeTemplate(Frame, V, Prepared);
    }
    // Indices are the existing LAM::CurveNames contract; verified in automation tests.
    const float Open =
        float(FMath::Clamp(
            double(V[24]) + double(S.LipOpenContribution) * ((V[47] + V[48]) * .5 + (V[33] + V[34]) * .5), 0.0, 1.0)) *
        (1.f - V[26]);
    const float Width =
        Smooth(S.Width, FMath::Max((V[45] + V[46]) * .5f, S.SmileContribution * ((V[43] + V[44]) * .5f)));
    const float Round = Smooth(S.Roundness, FMath::Max(V[31], V[37]));
    const float G = Smooth(S.Activation, Open), H = Smooth(S.OpenSplit, Open);
    // Double intermediates avoid overflow for very large but finite user gains.
    double Vowels[] = {double(G) * (1 - Round) * (1 - Width) * S.VowelGains.A,
                       double(G) * (1 - Round) * Width * (1 - H) * S.VowelGains.I,
                       double(G) * Round * (1 - H) * S.VowelGains.U,
                       double(G) * (1 - Round) * Width * H * S.VowelGains.E, double(G) * Round * H * S.VowelGains.O};
    double Sum = 0;
    for (double Value : Vowels)
        Sum += Value;
    const double Divisor = FMath::Max(1.0, Sum);
    constexpr int32 Slots[] = {10, 12, 14, 11, 13};
    float Total = 0;
    for (int32 I = 0; I < 5; ++I)
    {
        Result.Values[Slots[I]] = float(Vowels[I] / Divisor);
        Total += Result.Values[Slots[I]];
    }
    Result.Values[0] = FMath::Clamp(1.f - Total, 0.f, 1.f);
    Result.TimeSeconds = Frame.TimeSeconds;
    Result.Weight = FMath::Clamp(Frame.Weight, 0.f, 1.f);
    Result.bValid = true;
    return Result;
}
FLAMVisemeFrame ULAMVisemeLibrary::ConvertARKitToVisemes(const FLAMExpressionFrame &Frame,
                                                         const ULAMVisemeProfile *Profile)
{
    TArray<FString> Errors;
    if (Profile && !Profile->ValidateProfile(Errors))
        return {};
    return LAM::ConvertARKitToVisemes(Frame, Profile ? Profile->Settings : FLAMVisemeSettings());
}
float ULAMVisemeLibrary::GetVisemeWeight(const FLAMVisemeFrame &Frame, ELAMViseme Viseme)
{
    const int32 I = static_cast<uint8>(Viseme);
    return Frame.bValid && Frame.Values.Num() == LAM::VisemeCount && Frame.Values.IsValidIndex(I) &&
                   FMath::IsFinite(Frame.Values[I])
               ? FMath::Clamp(Frame.Values[I], 0.f, 1.f)
               : 0.f;
}
FLAMVowelWeights ULAMVisemeLibrary::GetVowelWeights(const FLAMVisemeFrame &Frame)
{
    FLAMVowelWeights R;
    R.A = GetVisemeWeight(Frame, ELAMViseme::aa);
    R.I = GetVisemeWeight(Frame, ELAMViseme::ih);
    R.U = GetVisemeWeight(Frame, ELAMViseme::ou);
    R.E = GetVisemeWeight(Frame, ELAMViseme::E);
    R.O = GetVisemeWeight(Frame, ELAMViseme::oh);
    return R;
}
void ULAMVisemeLibrary::ApplyUpperFaceOnlyPreset(ULAMCurveProfile *Profile)
{
    if (!Profile)
        return;
    Profile->Rules.Reset();
    for (const FName Name : LAM::CurveNames())
    {
        const FString Text = Name.ToString();
        if (Text.StartsWith(TEXT("jaw")) || Text.StartsWith(TEXT("mouth")) || Text == TEXT("tongueOut"))
        {
            FLAMCurveRule Rule;
            Rule.SourceName = Name;
            Rule.bEnabled = false;
            Profile->Rules.Add(Rule);
        }
    }
}
