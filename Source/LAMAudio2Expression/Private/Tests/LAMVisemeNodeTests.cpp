#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "AnimNode_LAMViseme.h"
#include "AnimNode_LAMARKit.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "HAL/UnrealMemory.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMVisemeNodeTest, "LAM.Viseme.AnimNodeIntegration",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMVisemeNodeTest::RunTest(const FString &)
{
    FMemMark Mark(FMemStack::Get());
    auto *Mesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Engine/EngineMeshes/SkeletalCube.SkeletalCube"));
    if (!TestNotNull(TEXT("Engine test mesh"), Mesh))
        return false;
    auto *Component = NewObject<USkeletalMeshComponent>();
    Component->SetSkeletalMesh(Mesh);
    auto *Instance = NewObject<UAnimInstance>(Component);
    struct FTestProxy : FAnimInstanceProxy
    {
        using FAnimInstanceProxy::FAnimInstanceProxy;
        using FAnimInstanceProxy::Initialize;
    } Proxy(Instance);
    Proxy.Initialize(Instance);
    TArray<FBoneIndexType> Bones;
    for (int32 I = 0; I < Mesh->GetRefSkeleton().GetNum(); ++I)
        Bones.Add(I);
    Proxy.GetRequiredBones().InitializeTo(Bones, UE::Anim::FCurveFilterSettings(), *Mesh);
    struct FSource : FAnimNode_Base
    {
        void Evaluate_AnyThread(FPoseContext &Out) override
        {
            Out.ResetToRefPose();
            Out.Curve.Set(TEXT("A"), .2f);
            Out.Curve.Set(TEXT("eyeBlinkLeft"), .7f);
        }
    } Source;
    auto *Profile = NewObject<ULAMVisemeProfile>();
    FAnimNode_LAMViseme Node;
    Node.Profile = Profile;
    Node.SourcePose.SetLinkNode(&Source);
    Node.Initialize_AnyThread(FAnimationInitializeContext(&Proxy));
    Node.CacheBones_AnyThread(FAnimationCacheBonesContext(&Proxy));
    Node.PreUpdate(Instance);
    FLAMExpressionFrame Frame;
    Frame.Values.Init(0, 52);
    Frame.Values[24] = 1;
    Frame.bValid = true;
    Frame.Weight = .5f;
    Node.Snapshot = Frame;
    Node.bSourceAvailable = true;
    Node.Alpha = .5f;
    auto Evaluate = [&]()
    {
        Node.Update_AnyThread(FAnimationUpdateContext(&Proxy, 1.f / 60));
        FPoseContext Out(&Proxy);
        Node.Evaluate_AnyThread(Out);
        TestEqual(TEXT("Unrelated eye preserved"), Out.Curve.Get(TEXT("eyeBlinkLeft")), .7f);
        return Out.Curve.Get(TEXT("A"));
    };
    TestTrue(TEXT("Alpha and frame weight applied once to base"), FMath::IsNearlyEqual(Evaluate(), .4f));
    Node.Alpha = 0;
    TestEqual(TEXT("Alpha zero passes through"), Evaluate(), .2f);
    Node.Alpha = 1;
    Node.Snapshot.Weight = 1;
    TestEqual(TEXT("BP conversion agrees with actual node"), Evaluate(),
              ULAMVisemeLibrary::ConvertARKitToVisemes(Node.Snapshot, Profile).Values[10]);
    TestEqual(TEXT("Unchanged paused snapshot holds"), Evaluate(), 1.f);
    Node.Snapshot.TimeSeconds = .1f;
    Node.Snapshot.Values[24] = 0;
    TestEqual(TEXT("Seek responds immediately"), Evaluate(), 0.f);
    Node.Snapshot = Frame;
    Node.Snapshot.Weight = 1;
    Node.PreUpdate(Instance); // No owner/component: cached snapshot fades on the worker update.
    Node.Update_AnyThread(FAnimationUpdateContext(&Proxy, .05f));
    TestTrue(TEXT("Lost source half fade at 50ms"), FMath::IsNearlyEqual(Node.Snapshot.Weight, .5f));
    Node.Update_AnyThread(FAnimationUpdateContext(&Proxy, .05f));
    TestFalse(TEXT("Lost source invalid after 100ms"), Node.Snapshot.bValid);
    TestEqual(TEXT("Lost source restores input pose"), Evaluate(), .2f);
    Node.bSourceAvailable = true;
    Node.Snapshot = Frame;
    Node.Snapshot.Weight = 0;
    TestEqual(TEXT("Stopped source restores input pose"), Evaluate(), .2f);
    Profile->TargetNames[ELAMViseme::aa] = TEXT("MissingMorphOnCube");
    Profile->TargetNames[ELAMViseme::ih] = NAME_None;
    Node.PreUpdate(Instance);
    Node.bSourceAvailable = true;
    Node.Snapshot = Frame;
    TestEqual(TEXT("Missing/unassigned targets do not overwrite A"), Evaluate(), .2f);
    // PreUpdate copies the profile; subsequent game-thread edits cannot mutate this evaluation.
    Profile->TargetNames[ELAMViseme::aa] = TEXT("A");
    TestEqual(TEXT("Profile is snapshotted"), Evaluate(), .2f);
    for (const auto Template : {ELAMVisemeTemplate::OpenFaceFX, ELAMVisemeTemplate::TalkingHead})
    {
        Profile->Settings.ConversionMode = ELAMVisemeConversionMode::TemplateFit;
        Profile->Settings.Template = Template;
        Profile->TargetNames.Reset();
        Profile->TargetNames.Add(ELAMViseme::PP, TEXT("A"));
        Node.PreUpdate(Instance);
        Frame.Values.Init(0, 52);
        const auto &Basis = LAM::VisemeBasis(Template);
        for (int32 I = 0; I < 52; ++I)
            Frame.Values[I] = float(Basis.Values[I][0]);
        Frame.Weight = .5f;
        Node.Snapshot = Frame;
        Node.bSourceAvailable = true;
        Node.Alpha = .5f;
        const auto Expected = ULAMVisemeLibrary::ConvertARKitToVisemes(Frame, Profile);
        TestTrue(TEXT("Template BP/node agree; alpha/weight applied once"),
                 FMath::IsNearlyEqual(Evaluate(), FMath::Lerp(.2f, Expected.Values[1], .25f), 1.e-6f));
        Profile->Settings.VisemeGains.Add(ELAMViseme::PP, 0);
        TestTrue(TEXT("Prepared settings remain immutable until PreUpdate"),
                 FMath::IsNearlyEqual(Evaluate(), FMath::Lerp(.2f, Expected.Values[1], .25f), 1.e-6f));
        Node.PreUpdate(Instance);
        Node.bSourceAvailable = true;
        TestTrue(TEXT("Profile changes rebuild prepared settings"), FMath::IsNearlyEqual(Evaluate(), .15f));
        Profile->Settings.VisemeGains.Reset();
        Node.PreUpdate(Instance);
        Node.bSourceAvailable = true;
        Node.Snapshot = Frame;
        const float Held = Evaluate();
        TestEqual(TEXT("Template pause holds"), Evaluate(), Held);
        Node.Snapshot.Values.Init(0, 52);
        TestTrue(TEXT("Template seek is immediate"), FMath::IsNearlyEqual(Evaluate(), .15f));
        Node.Snapshot = Frame;
        Node.PreUpdate(Instance);
        Node.Update_AnyThread(FAnimationUpdateContext(&Proxy, .1f));
        TestEqual(TEXT("Template source loss restores base"), Evaluate(), .2f);
    }
    return true;
}
#endif
