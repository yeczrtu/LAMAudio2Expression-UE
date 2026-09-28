#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "LAMViseme.h"
#include "AnimNode_LAMViseme.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct LAMAUDIO2EXPRESSION_API FAnimNode_LAMViseme : public FAnimNode_Base
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Links") FPoseLink SourcePose;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM", meta = (PinShownByDefault))
    TObjectPtr<class ULAMAudio2ExpressionComponent> SourceComponent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM", meta = (PinShownByDefault)) float Alpha = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LAM") TObjectPtr<ULAMVisemeProfile> Profile;
    void Initialize_AnyThread(const FAnimationInitializeContext &Context) override;
    void CacheBones_AnyThread(const FAnimationCacheBonesContext &Context) override;
    void Update_AnyThread(const FAnimationUpdateContext &Context) override;
    void Evaluate_AnyThread(FPoseContext &Output) override;
    bool HasPreUpdate() const override
    {
        return true;
    }
    void PreUpdate(const UAnimInstance *Instance) override;

  private:
    friend class FLAMVisemeNodeTest;
    FLAMExpressionFrame Snapshot;
    LAM::FPreparedVisemeSettings Prepared;
    TMap<ELAMViseme, FName> Names;
    bool bSourceAvailable = false;
    bool bProfileValid = true;
    bool bReportedInvalidProfile = false;
};
