#include "AnimNode_LAMViseme.h"
#include "LAMAudio2ExpressionComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Actor.h"

void FAnimNode_LAMViseme::Initialize_AnyThread(const FAnimationInitializeContext &C)
{
    Snapshot = {};
    bSourceAvailable = false;
    SourcePose.Initialize(C);
}
void FAnimNode_LAMViseme::CacheBones_AnyThread(const FAnimationCacheBonesContext &C)
{
    SourcePose.CacheBones(C);
}
void FAnimNode_LAMViseme::Update_AnyThread(const FAnimationUpdateContext &C)
{
    GetEvaluateGraphExposedInputs().Execute(C);
    SourcePose.Update(C);
    if (!bSourceAvailable)
    {
        Snapshot.Weight = FMath::Max(0.f, Snapshot.Weight - C.GetDeltaTime() / .1f);
        if (Snapshot.Weight == 0)
            Snapshot.bValid = false;
    }
}
void FAnimNode_LAMViseme::PreUpdate(const UAnimInstance *Instance)
{
    auto *Component = SourceComponent.Get();
    if (!Component && Instance->GetOwningActor())
        Component = Instance->GetOwningActor()->FindComponentByClass<ULAMAudio2ExpressionComponent>();
    bSourceAvailable = IsValid(Component);
    if (bSourceAvailable)
        Snapshot = Component->GetCurrentExpressionFrame();
    const auto *ActiveProfile = IsValid(Profile) ? Profile.Get() : GetDefault<ULAMVisemeProfile>();
    if (!Prepared.bValid || !FLAMVisemeSettings::StaticStruct()->CompareScriptStruct(
                                &Prepared.Settings, &ActiveProfile->Settings, 0))
        Prepared = LAM::PrepareVisemeSettings(ActiveProfile->Settings);
    Names = ActiveProfile->TargetNames;
    TArray<FString> Errors;
    bProfileValid = ActiveProfile->ValidateProfile(Errors);
    if (!bProfileValid && !bReportedInvalidProfile)
        UE_LOG(LogTemp, Warning, TEXT("LAM Viseme profile invalid: %s"), *FString::Join(Errors, TEXT("; ")));
    bReportedInvalidProfile = !bProfileValid;
}
void FAnimNode_LAMViseme::Evaluate_AnyThread(FPoseContext &Out)
{
    SourcePose.Evaluate(Out);
    if (!bProfileValid || !FMath::IsFinite(Alpha))
        return;
    const auto Frame = LAM::ConvertARKitToVisemes(Snapshot, Prepared);
    if (!Frame.bValid)
        return;
    const float Weight = FMath::Clamp(Alpha * Frame.Weight, 0.f, 1.f);
    if (Weight <= 0)
        return;
    for (const auto &Binding : Names)
        if (!Binding.Value.IsNone())
            Out.Curve.Set(Binding.Value, FMath::Lerp(Out.Curve.Get(Binding.Value),
                                                     Frame.Values[static_cast<uint8>(Binding.Key)], Weight));
}
