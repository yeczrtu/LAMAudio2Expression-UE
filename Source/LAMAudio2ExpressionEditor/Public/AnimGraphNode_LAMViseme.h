#pragma once
#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_LAMViseme.h"
#include "AnimGraphNode_LAMViseme.generated.h"

UCLASS(meta = (Keywords = "Lipsync"))
class LAMAUDIO2EXPRESSIONEDITOR_API UAnimGraphNode_LAMViseme : public UAnimGraphNode_Base
{
    GENERATED_BODY()
  public:
    UPROPERTY(EditAnywhere, Category = "Settings") FAnimNode_LAMViseme Node;
    FText GetNodeTitle(ENodeTitleType::Type Type) const override
    {
        return FText::FromString(TEXT("Apply LAM Viseme Curves"));
    }
    FText GetTooltipText() const override
    {
        return FText::FromString(TEXT(
            "Converts LAM ARKit curves to vowels or fitted Oculus visemes. Empty Source Component uses the owning actor. Null Profile "
            "uses A/I/U/E/O. TemplateFit enables consonants; sil is the neutral residual, not detected silence."));
    }
    FString GetNodeCategory() const override
    {
        return TEXT("LAM Audio2Expression");
    }
};
