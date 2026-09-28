#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LAMEditorLibrary.generated.h"
UCLASS()
class LAMAUDIO2EXPRESSIONEDITOR_API ULAMEditorLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
  public:
    UFUNCTION(BlueprintCallable, Category = "LAM|Editor") static bool CreateExamples();
    UFUNCTION(BlueprintCallable, Category = "LAM|Editor")
    static bool CreateBakedExample(class ULAMBakedExpressionClip* Clip);
    // Creates presets and an upper-face + vowel AnimBP without changing existing assets.
    UFUNCTION(BlueprintCallable, Category = "LAM|Editor") static bool CreateVisemeExamples(class USkeletalMesh *Mesh);
    UFUNCTION(BlueprintCallable, Category = "LAM|Editor") static bool CreateOculusExamples(class USkeletalMesh *Mesh);
};
