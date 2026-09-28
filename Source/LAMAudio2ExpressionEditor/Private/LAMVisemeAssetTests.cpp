#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "LAMViseme.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/MorphTarget.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLAMVisemeAssetTest, "LAM.Editor.VisemeMeshRecipes",
                               EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLAMVisemeAssetTest::RunTest(const FString &)
{
    auto *Source = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/LAMFaceDemo/Character/Face52"), nullptr, LOAD_NoWarn);
    if (!Source)
    {
        AddInfo(TEXT("Optional demo meshes absent; recipe mesh test is only applicable to the demo project."));
        return true;
    }
    TArray<FSoftSkinVertex> SourceVertices;
    Source->GetImportedModel()->LODModels[0].GetVertices(SourceVertices);
    for (const auto Template : {ELAMVisemeTemplate::OpenFaceFX, ELAMVisemeTemplate::TalkingHead})
    {
        const FString Name = Template == ELAMVisemeTemplate::OpenFaceFX ? TEXT("OpenFaceFX") : TEXT("TalkingHead");
        auto *Mesh = LoadObject<USkeletalMesh>(nullptr, *(TEXT("/Game/LAMVisemeExamples/SK_Visemes") + Name));
        auto *Profile = LoadObject<ULAMVisemeProfile>(nullptr, *(TEXT("/LAMAudio2Expression/Profiles/DA_Oculus") + Name));
        if (!TestNotNull(TEXT("Reference mesh"), Mesh) || !TestNotNull(TEXT("Reference profile"), Profile))
            return false;
        TArray<FSoftSkinVertex> Vertices;
        Mesh->GetImportedModel()->LODModels[0].GetVertices(Vertices);
        if (!TestEqual(TEXT("Cloned vertex count"), Vertices.Num(), SourceVertices.Num()))
            return false;
        float BaseError = 0;
        for (int32 I = 0; I < Vertices.Num(); ++I)
            BaseError = FMath::Max(BaseError, (Vertices[I].Position - SourceVertices[I].Position).Size());
        TestTrue(TEXT("Cloned vertex order and positions preserved"), BaseError < 1.e-6f);
        const auto &B = LAM::VisemeBasis(Template);
        for (int32 J = 0; J < 14; ++J)
        {
            TMap<uint32, FVector3f> Expected, Actual;
            double ExpectedEnergy = 0, MaxError = 0;
            for (int32 I = 0; I < 52; ++I)
            {
                if (B.Values[I][J] == 0)
                    continue;
                auto *Morph = Source->FindMorphTarget(LAM::CurveNames()[I]);
                if (!TestNotNull(TEXT("Source ARKit morph"), Morph))
                    return false;
                for (const auto &Delta : Morph->GetMorphTargetDeltas(0))
                    Expected.FindOrAdd(Delta.SourceIdx, FVector3f::ZeroVector) += Delta.PositionDelta * float(B.Values[I][J]);
            }
            auto *Morph = Mesh->FindMorphTarget(Profile->TargetNames[static_cast<ELAMViseme>(J + 1)]);
            if (!TestNotNull(TEXT("Generated morph"), Morph))
                return false;
            for (const auto &Delta : Morph->GetMorphTargetDeltas(0))
                Actual.Add(Delta.SourceIdx, Delta.PositionDelta);
            for (const auto &Pair : Expected)
            {
                const auto *Value = Actual.Find(Pair.Key);
                MaxError = FMath::Max(MaxError, double((Pair.Value - (Value ? *Value : FVector3f::ZeroVector)).Size()));
                ExpectedEnergy += Pair.Value.SizeSquared();
            }
            for (const auto &Pair : Actual)
                if (!Expected.Contains(Pair.Key))
                    MaxError = FMath::Max(MaxError, double(Pair.Value.Size()));
            TestTrue(TEXT("Saved morph equals forward recipe at every vertex"), MaxError < 1.e-5);
            AddInfo(FString::Printf(TEXT("%s slot %d: vertices %d, delta RMS %.6g cm, maximum error %.6g cm"),
                                   *Name, J + 1, Actual.Num(), FMath::Sqrt(ExpectedEnergy / Vertices.Num()), MaxError));
        }
    }
    return true;
}
#endif
