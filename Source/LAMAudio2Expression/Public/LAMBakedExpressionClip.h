#pragma once
#include "CoreMinimal.h"
#include "LAMTypes.h"
#include "LAMBakedExpressionClip.generated.h"

UCLASS(BlueprintType)
class LAMAUDIO2EXPRESSION_API ULAMBakedExpressionClip : public ULAMExpressionClip
{
    GENERATED_BODY()
  public:
    static constexpr int32 CurrentFormatVersion = 1;
    static constexpr int32 CurrentProcessingVersion = 1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LAM|Bake") int32 FormatVersion = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LAM|Bake") int32 ProcessingVersion = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LAM|Bake") FGuid SourceGuid;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LAM|Bake") FGuid ModelGuid;
    // A string deliberately avoids adding a cook dependency on the inference model.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LAM|Bake") FString ModelPath;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LAM|Bake") FString PCMHash;

    void PostLoad() override;
    bool ValidateBakedData(FString &Error) const;
    void RefreshValidation();
    const FString &GetPlaybackValidationError() const
    {
        return ValidationError;
    }
#if WITH_EDITOR
    bool NeedsRegeneration(FString &Reason) const;
    EDataValidationResult IsDataValid(class FDataValidationContext &Context) const override;
    void PostEditUndo() override;
#endif
  private:
    FString ValidationError = TEXT("The clip has not been baked.");
};
