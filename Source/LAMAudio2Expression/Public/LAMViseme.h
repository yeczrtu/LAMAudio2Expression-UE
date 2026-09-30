#pragma once
#include "CoreMinimal.h"
#include "LAMTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LAMViseme.generated.h"

// Numeric values are the Oculus SDK slots, not alphabetical order.
UENUM(BlueprintType)
enum class ELAMViseme : uint8
{
    sil,
    PP,
    FF,
    TH,
    DD,
    kk,
    CH,
    SS,
    nn,
    RR,
    aa,
    E,
    ih,
    oh,
    ou
};

UENUM(BlueprintType)
enum class ELAMVisemePreset : uint8
{
    JapaneseFive,
    OculusReference,
    OculusSDK,
    OculusPrefixed
};

UENUM(BlueprintType)
enum class ELAMVisemeConversionMode : uint8
{
    FiveVowelRules,
    TemplateFit
};

UENUM(BlueprintType)
enum class ELAMVisemeTemplate : uint8
{
    OpenFaceFX,
    TalkingHead
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMVowelWeights
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0")) float A = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0")) float I = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0")) float U = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0")) float E = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0")) float O = 0;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMVisemeRange
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0", ClampMax = "1"))
    float Low = .1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0", ClampMax = "1"))
    float High = .6f;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMVisemeInputCorrection
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme") FName SourceName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0", ClampMax = "1"))
    float Baseline = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0")) float Scale = 1;
    // Template fitting only: confidence in this observation, not an input multiplier.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0", ClampMax = "1"))
    float FitWeight = 1;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMVisemeSettings
{
    GENERATED_BODY()
    FLAMVisemeSettings();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme")
    ELAMVisemeConversionMode ConversionMode = ELAMVisemeConversionMode::FiveVowelRules;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme",
              meta = (EditCondition = "ConversionMode == ELAMVisemeConversionMode::TemplateFit", EditConditionHides))
    ELAMVisemeTemplate Template = ELAMVisemeTemplate::OpenFaceFX;
    // Unlisted non-neutral slots have gain 1. sil is always the residual.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme",
              meta = (EditCondition = "ConversionMode == ELAMVisemeConversionMode::TemplateFit", EditConditionHides))
    TMap<ELAMViseme, float> VisemeGains;
    // Unlisted ARKit curves have baseline 0 and scale 1.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme")
    TArray<FLAMVisemeInputCorrection> InputCorrections;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0", EditCondition = "ConversionMode == ELAMVisemeConversionMode::FiveVowelRules", EditConditionHides))
    float LipOpenContribution = .25f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (ClampMin = "0", EditCondition = "ConversionMode == ELAMVisemeConversionMode::FiveVowelRules", EditConditionHides))
    float SmileContribution = .15f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (EditCondition = "ConversionMode == ELAMVisemeConversionMode::FiveVowelRules", EditConditionHides))
    FLAMVisemeRange Width;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (EditCondition = "ConversionMode == ELAMVisemeConversionMode::FiveVowelRules", EditConditionHides))
    FLAMVisemeRange Roundness;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (EditCondition = "ConversionMode == ELAMVisemeConversionMode::FiveVowelRules", EditConditionHides))
    FLAMVisemeRange Activation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme", meta = (EditCondition = "ConversionMode == ELAMVisemeConversionMode::FiveVowelRules", EditConditionHides))
    FLAMVisemeRange OpenSplit;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme",
              meta = (EditCondition = "ConversionMode == ELAMVisemeConversionMode::FiveVowelRules", EditConditionHides))
    FLAMVowelWeights VowelGains;
};

USTRUCT(BlueprintType)
struct LAMAUDIO2EXPRESSION_API FLAMVisemeFrame
{
    GENERATED_BODY()
    FLAMVisemeFrame()
    {
        Values.Init(0.f, 15);
    }
    UPROPERTY(BlueprintReadOnly, Category = "LAM|Viseme") float TimeSeconds = 0;
    // Always 15 slots in ELAMViseme order. Weight is NOT multiplied into these values.
    UPROPERTY(BlueprintReadOnly, Category = "LAM|Viseme") TArray<float> Values;
    UPROPERTY(BlueprintReadOnly, Category = "LAM|Viseme") bool bValid = false;
    UPROPERTY(BlueprintReadOnly, Category = "LAM|Viseme") float Weight = 0;
};

UCLASS(BlueprintType)
class LAMAUDIO2EXPRESSION_API ULAMVisemeProfile : public UDataAsset
{
    GENERATED_BODY()
  public:
    ULAMVisemeProfile();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme") FLAMVisemeSettings Settings;
    // Missing / None entries do not write a curve. Names must be unique.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM|Viseme") TMap<ELAMViseme, FName> TargetNames;
    UFUNCTION(BlueprintCallable, Category = "LAM|Viseme", meta = (Keywords = "Lipsync")) void ApplyNamePreset(ELAMVisemePreset Preset);
    UFUNCTION(BlueprintPure, Category = "LAM|Viseme", meta = (Keywords = "Lipsync")) bool ValidateProfile(TArray<FString> &Errors) const;
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(class FDataValidationContext &Context) const override;
#endif
};

UCLASS()
class LAMAUDIO2EXPRESSION_API ULAMVisemeLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
  public:
    // Null Profile uses the default rule settings (and JapaneseFive when applied in AnimGraph).
    UFUNCTION(BlueprintPure, Category = "LAM|Viseme", meta = (Keywords = "Lipsync"))
    static FLAMVisemeFrame ConvertARKitToVisemes(const FLAMExpressionFrame &Frame, const ULAMVisemeProfile *Profile);
    UFUNCTION(BlueprintPure, Category = "LAM|Viseme", meta = (Keywords = "Lipsync"))
    static FLAMVowelWeights GetVowelWeights(const FLAMVisemeFrame &Frame);
    UFUNCTION(BlueprintPure, Category = "LAM|Viseme", meta = (Keywords = "Lipsync"))
    static float GetVisemeWeight(const FLAMVisemeFrame &Frame, ELAMViseme Viseme);
    UFUNCTION(BlueprintPure, Category = "LAM|Viseme", meta = (Keywords = "Lipsync")) static TArray<FName> GetVisemeNames();
    // Replaces the rule list; use on a separate profile for the upstream ARKit node.
    UFUNCTION(BlueprintCallable, Category = "LAM|Viseme", meta = (Keywords = "Lipsync"))
    static void ApplyUpperFaceOnlyPreset(ULAMCurveProfile *Profile);
};

namespace LAM
{
constexpr int32 VisemeCount = 15;
constexpr int32 FittedVisemeCount = VisemeCount - 1;
struct FVisemeBasis
{
    double Values[CurveCount][FittedVisemeCount] = {};
};
// Pinned forward recipes, columns PP..ou, rows CurveNames(). No UObject references.
LAMAUDIO2EXPRESSION_API const FVisemeBasis &VisemeBasis(ELAMVisemeTemplate Template);
struct FPreparedVisemeSettings
{
    FLAMVisemeSettings Settings;
    double H[FittedVisemeCount][FittedVisemeCount] = {};
    double ObservationWeights[CurveCount] = {};
    double Lipschitz = 1;
    bool bValid = false;
};
// Prepare on the game thread when settings change; evaluations only read this snapshot.
LAMAUDIO2EXPRESSION_API FPreparedVisemeSettings PrepareVisemeSettings(const FLAMVisemeSettings &Settings);
LAMAUDIO2EXPRESSION_API FLAMVisemeFrame ConvertARKitToVisemes(const FLAMExpressionFrame &Frame,
                                                           const FPreparedVisemeSettings &Prepared);
LAMAUDIO2EXPRESSION_API bool ValidateVisemeSettings(const FLAMVisemeSettings &Settings, TArray<FString> &Errors);
// Value-only API: safe for worker threads, never dereferences a UObject.
LAMAUDIO2EXPRESSION_API FLAMVisemeFrame ConvertARKitToVisemes(const FLAMExpressionFrame &Frame,
                                                              const FLAMVisemeSettings &Settings);
} // namespace LAM
