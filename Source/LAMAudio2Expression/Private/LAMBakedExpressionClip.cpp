#include "LAMBakedExpressionClip.h"
#include "Sound/SoundWave.h"
#if WITH_EDITOR
#include "LAMSettings.h"
#include "Misc/DataValidation.h"
#endif

bool ULAMBakedExpressionClip::ValidateBakedData(FString &Error) const
{
    Error.Reset();
    if (FormatVersion != CurrentFormatVersion)
        Error = TEXT("Unsupported baked clip format. Regenerate the clip.");
    else if (!SoundWave || !FMath::IsFinite(Duration) || Duration <= 0 || Duration > 300.01f || NumSamples <= 0 ||
             NumSamples > LAM::Rate * 300 || FrameRate != LAM::FPS ||
             FMath::Abs(Duration - float(NumSamples) / LAM::Rate) > 2.f / LAM::Rate ||
             Curves.Num() != ((int64(NumSamples) * LAM::FPS + LAM::Rate - 1) / LAM::Rate) * LAM::CurveCount)
        Error = TEXT("Invalid baked clip dimensions or missing source sound. Regenerate the clip.");
    else
        for (float Value : Curves)
            if (!FMath::IsFinite(Value))
            {
                Error = TEXT("Baked curves contain a non-finite value. Regenerate the clip.");
                break;
            }
    return Error.IsEmpty();
}
void ULAMBakedExpressionClip::RefreshValidation()
{
    ValidateBakedData(ValidationError);
}
void ULAMBakedExpressionClip::PostLoad()
{
    Super::PostLoad();
    RefreshValidation();
}
#if WITH_EDITOR
void ULAMBakedExpressionClip::PostEditUndo()
{
    Super::PostEditUndo();
    RefreshValidation();
}
bool ULAMBakedExpressionClip::NeedsRegeneration(FString &Reason) const
{
    if (!ValidateBakedData(Reason))
        return true;
    if (ProcessingVersion != CurrentProcessingVersion || !SourceGuid.IsValid() ||
        SourceGuid != SoundWave->CompressedDataGuid)
        Reason = TEXT("The source audio or processing version changed. Regenerate the clip.");
    else
    {
        const auto &ConfiguredModel = GetDefault<ULAMSettings>()->Model;
        const auto *Model = ConfiguredModel.LoadSynchronous();
        if (!Model || ModelPath != ConfiguredModel.ToSoftObjectPath().ToString() || ModelGuid != Model->GetFileId())
            Reason = TEXT("The analysis model changed or is unavailable. Regenerate with the configured model.");
    }
    return !Reason.IsEmpty();
}
EDataValidationResult ULAMBakedExpressionClip::IsDataValid(FDataValidationContext &Context) const
{
    FString Error;
    if (NeedsRegeneration(Error))
    {
        Context.AddError(FText::FromString(Error));
        return EDataValidationResult::Invalid;
    }
    return EDataValidationResult::Valid;
}
#endif
