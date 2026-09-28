#include "LAMEditorLibrary.h"
#include "LAMAnalyzeAsync.h"
#include "LAMAudio2ExpressionComponent.h"
#include "AnimGraphNode_LAMARKit.h"
#include "AnimGraphNode_LAMViseme.h"
#include "Engine/SkeletalMesh.h"
#include "AnimGraphNode_Root.h"
#include "AnimGraphNode_LocalRefPose.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/Skeleton.h"
#include "Animation/MorphTarget.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "Sound/SoundWave.h"
#include "AnimationGraphSchema.h"
#include "Factories/AnimBlueprintFactory.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "K2Node_Event.h"
#include "K2Node_AsyncAction.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "EdGraphSchema_K2.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "GameFramework/Actor.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "NNEModelData.h"

bool ULAMEditorLibrary::ConfigureCPUModel(UNNEModelData* Model)
{
    if (!Model)
        return false;
    const TArray<FString> Runtimes = {TEXT("NNERuntimeORTCpu")};
    Model->SetTargetRuntimes(Runtimes);
    Model->MarkPackageDirty();
    return true;
}

static bool SaveLAMAsset(UObject *Object)
{
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    Args.SaveFlags = SAVE_NoError;
    return UPackage::SavePackage(Object->GetOutermost(), Object,
                                 *FPackageName::LongPackageNameToFilename(Object->GetOutermost()->GetName(),
                                                                          FPackageName::GetAssetPackageExtension()),
                                 Args);
}
static bool CreateVisemeAnimBlueprint(USkeletalMesh *Mesh, ULAMVisemeProfile *Profile,
                                     ULAMCurveProfile *Upper, const FString &BPPath)
{
    if (FPackageName::DoesPackageExist(BPPath))
        return true;
    auto *Factory = NewObject<UAnimBlueprintFactory>();
    Factory->TargetSkeleton = Mesh->GetSkeleton();
    auto *BP = Cast<UAnimBlueprint>(Factory->FactoryCreateNew(UAnimBlueprint::StaticClass(), CreatePackage(*BPPath),
                                                              *FPackageName::GetShortName(BPPath), RF_Public | RF_Standalone,
                                                              nullptr, GWarn));
    BP->SetPreviewMesh(Mesh);
    TArray<UEdGraph *> Graphs;
    BP->GetAllGraphs(Graphs);
    UEdGraph *Graph = nullptr;
    UAnimGraphNode_Root *Root = nullptr;
    for (auto *G : Graphs)
        for (const auto &N : G->Nodes)
            if (auto *R = Cast<UAnimGraphNode_Root>(N.Get()))
            {
                Graph = G;
                Root = R;
            }
    if (!Graph || !Root)
        return false;
    FGraphNodeCreator<UAnimGraphNode_LocalRefPose> RefCreator(*Graph);
    auto *Ref = RefCreator.CreateNode();
    RefCreator.Finalize();
    Ref->NodePosX = -750;
    FGraphNodeCreator<UAnimGraphNode_LAMARKit> UpperCreator(*Graph);
    auto *UpperNode = UpperCreator.CreateNode();
    UpperNode->Node.CurveProfile = Upper;
    UpperCreator.Finalize();
    UpperNode->NodePosX = -500;
    FGraphNodeCreator<UAnimGraphNode_LAMViseme> Creator(*Graph);
    auto *Node = Creator.CreateNode();
    Node->Node.Profile = Profile;
    Creator.Finalize();
    Node->NodePosX = -250;
    auto PosePin = [](UEdGraphNode *N, EEdGraphPinDirection D) -> UEdGraphPin *
    {
        for (auto *Pin : N->Pins)
            if (Pin->Direction == D && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
                return Pin;
        return nullptr;
    };
    const auto *Schema = Graph->GetSchema();
    if (!Schema->TryCreateConnection(PosePin(Ref, EGPD_Output), UpperNode->FindPinChecked(TEXT("SourcePose"))) ||
        !Schema->TryCreateConnection(PosePin(UpperNode, EGPD_Output), Node->FindPinChecked(TEXT("SourcePose"))) ||
        !Schema->TryCreateConnection(PosePin(Node, EGPD_Output), PosePin(Root, EGPD_Input)))
        return false;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    return BP->Status != BS_Error && SaveLAMAsset(BP);
}
bool ULAMEditorLibrary::CreateVisemeExamples(USkeletalMesh *Mesh)
{
    if (!Mesh || !Mesh->GetSkeleton())
        return false;
    for (const auto Preset :
         {ELAMVisemePreset::JapaneseFive, ELAMVisemePreset::OculusReference, ELAMVisemePreset::OculusSDK})
    {
        const FString Name = Preset == ELAMVisemePreset::JapaneseFive      ? TEXT("DA_JapaneseFive")
                             : Preset == ELAMVisemePreset::OculusReference ? TEXT("DA_OculusReference")
                                                                           : TEXT("DA_OculusSDK");
        const FString Path = TEXT("/LAMAudio2Expression/Profiles/") + Name;
        if (!FPackageName::DoesPackageExist(Path))
        {
            auto *Asset = NewObject<ULAMVisemeProfile>(CreatePackage(*Path), *Name, RF_Public | RF_Standalone);
            Asset->ApplyNamePreset(Preset);
            if (!SaveLAMAsset(Asset))
                return false;
        }
    }
    const FString UpperPath = TEXT("/LAMAudio2Expression/Profiles/DA_UpperFaceOnly");
    auto *Upper = LoadObject<ULAMCurveProfile>(nullptr, *(UpperPath + TEXT(".DA_UpperFaceOnly")), nullptr, LOAD_NoWarn);
    if (!Upper)
    {
        Upper =
            NewObject<ULAMCurveProfile>(CreatePackage(*UpperPath), TEXT("DA_UpperFaceOnly"), RF_Public | RF_Standalone);
        ULAMVisemeLibrary::ApplyUpperFaceOnlyPreset(Upper);
        if (!SaveLAMAsset(Upper))
            return false;
    }
    const FString ProfilePath = TEXT("/Game/LAMVisemeExamples/DA_VisemeMesh");
    auto *Profile =
        LoadObject<ULAMVisemeProfile>(nullptr, *(ProfilePath + TEXT(".DA_VisemeMesh")), nullptr, LOAD_NoWarn);
    if (!Profile)
    {
        Profile =
            NewObject<ULAMVisemeProfile>(CreatePackage(*ProfilePath), TEXT("DA_VisemeMesh"), RF_Public | RF_Standalone);
        for (auto &Pair : Profile->TargetNames)
        {
            const FName VRoidName(*(TEXT("Fcl_MTH_") + Pair.Value.ToString()));
            if (Mesh->FindMorphTarget(VRoidName))
                Pair.Value = VRoidName;
        }
        if (!SaveLAMAsset(Profile))
            return false;
    }
    const FString BPPath = TEXT("/Game/LAMVisemeExamples/ABP_LAMVisemes");
    if (FPackageName::DoesPackageExist(BPPath))
        return true;
    return CreateVisemeAnimBlueprint(Mesh, Profile, Upper, BPPath);
}

bool ULAMEditorLibrary::CreateOculusExamples(USkeletalMesh *SourceMesh)
{
    if (!SourceMesh || !SourceMesh->GetSkeleton() || !CreateVisemeExamples(SourceMesh))
        return false;
    auto *Upper = LoadObject<ULAMCurveProfile>(nullptr, TEXT("/LAMAudio2Expression/Profiles/DA_UpperFaceOnly"));
    for (const auto Template : {ELAMVisemeTemplate::OpenFaceFX, ELAMVisemeTemplate::TalkingHead})
    {
        const FString Suffix = Template == ELAMVisemeTemplate::OpenFaceFX ? TEXT("OpenFaceFX") : TEXT("TalkingHead");
        const FString ProfilePath = TEXT("/LAMAudio2Expression/Profiles/DA_Oculus") + Suffix;
        auto *Profile = LoadObject<ULAMVisemeProfile>(nullptr, *ProfilePath, nullptr, LOAD_NoWarn);
        if (!Profile)
        {
            Profile = NewObject<ULAMVisemeProfile>(CreatePackage(*ProfilePath),
                       *FPackageName::GetShortName(ProfilePath), RF_Public | RF_Standalone);
            Profile->Settings.ConversionMode = ELAMVisemeConversionMode::TemplateFit;
            Profile->Settings.Template = Template;
            Profile->ApplyNamePreset(Template == ELAMVisemeTemplate::OpenFaceFX ? ELAMVisemePreset::OculusSDK
                                                                             : ELAMVisemePreset::OculusPrefixed);
            if (!SaveLAMAsset(Profile))
                return false;
        }
        const FString MeshPath = TEXT("/Game/LAMVisemeExamples/SK_Visemes") + Suffix;
        auto *Mesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath, nullptr, LOAD_NoWarn);
        if (!Mesh)
        {
            Mesh = DuplicateObject<USkeletalMesh>(SourceMesh, CreatePackage(*MeshPath),
                                                 *FPackageName::GetShortName(MeshPath));
            Mesh->SetFlags(RF_Public | RF_Standalone);
            const auto *Model = Mesh->GetImportedModel();
            if (!Model)
                return false;
            const auto &B = LAM::VisemeBasis(Template);
            for (int32 J = 0; J < LAM::FittedVisemeCount; ++J)
            {
                auto *Morph = NewObject<UMorphTarget>(Mesh, Profile->TargetNames[static_cast<ELAMViseme>(J + 1)]);
                for (int32 LOD = 0; LOD < Model->LODModels.Num(); ++LOD)
                {
                    TMap<uint32, FMorphTargetDelta> Accumulated;
                    for (int32 I = 0; I < LAM::CurveCount; ++I)
                    {
                        if (B.Values[I][J] == 0)
                            continue;
                        const auto *Source = SourceMesh->FindMorphTarget(LAM::CurveNames()[I]);
                        if (!Source)
                            return false; // Do not silently generate incomplete reference shapes.
                        for (const auto &Delta : Source->GetMorphTargetDeltas(LOD))
                        {
                            auto &Combined = Accumulated.FindOrAdd(Delta.SourceIdx);
                            Combined.SourceIdx = Delta.SourceIdx;
                            Combined.PositionDelta += Delta.PositionDelta * float(B.Values[I][J]);
                            Combined.TangentZDelta += Delta.TangentZDelta * float(B.Values[I][J]);
                        }
                    }
                    TArray<FMorphTargetDelta> Deltas;
                    Accumulated.GenerateValueArray(Deltas);
                    Morph->PopulateDeltas(Deltas, LOD, Model->LODModels[LOD].Sections, true, false, 0.f);
                }
                if (!Mesh->RegisterMorphTarget(Morph, false))
                    return false;
            }
            Mesh->InvalidateDeriveDataCacheGUID();
            Mesh->InitMorphTargetsAndRebuildRenderData();
            if (!SaveLAMAsset(Mesh))
                return false;
        }
        if (!CreateVisemeAnimBlueprint(Mesh, Profile, Upper, TEXT("/Game/LAMVisemeExamples/ABP_") + Suffix))
            return false;
    }
    return true;
}
bool ULAMEditorLibrary::CreateExamples()
{
    if (auto *Model = LoadObject<UNNEModelData>(nullptr, TEXT("/LAMAudio2Expression/Models/LAM_A2E.LAM_A2E")))
    {
        TArray<FString> Runtimes = {TEXT("NNERuntimeORTCpu"), TEXT("NNERuntimeORTDml")};
        Model->SetTargetRuntimes(Runtimes);
        SaveLAMAsset(Model);
    }
    // These examples are created once. Never overwrite a user's edited graphs.
    if (!LoadObject<UBlueprint>(nullptr, TEXT("/Game/Examples/BP_LAMPlayback.BP_LAMPlayback"), nullptr, LOAD_NoWarn))
    {
        auto *Package = CreatePackage(TEXT("/Game/Examples/BP_LAMPlayback"));
        auto *BP = FKismetEditorUtilities::CreateBlueprint(AActor::StaticClass(), Package, TEXT("BP_LAMPlayback"),
                                                           BPTYPE_Normal, UBlueprint::StaticClass(),
                                                           UBlueprintGeneratedClass::StaticClass());
        auto *SCS = BP->SimpleConstructionScript.Get();
        SCS->AddNode(SCS->CreateNode(ULAMAudio2ExpressionComponent::StaticClass(), TEXT("LAM")));
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        FKismetEditorUtilities::CompileBlueprint(BP);
        auto *Graph = FBlueprintEditorUtils::FindEventGraph(BP);
        const auto *Schema = GetDefault<UEdGraphSchema_K2>();
        FGraphNodeCreator<UK2Node_Event> E(*Graph);
        auto *Begin = E.CreateNode();
        Begin->EventReference.SetExternalMember(TEXT("ReceiveBeginPlay"), AActor::StaticClass());
        Begin->bOverrideFunction = true;
        E.Finalize();
        Begin->NodePosX = 0;
        FGraphNodeCreator<UK2Node_VariableGet> V(*Graph);
        auto *Component = V.CreateNode();
        Component->VariableReference.SetSelfMember(TEXT("LAM"));
        V.Finalize();
        Component->NodePosY = 160;
        FGraphNodeCreator<UK2Node_AsyncAction> A(*Graph);
        auto *Analyze = A.CreateNode();
        Analyze->InitializeProxyFromFunction(ULAMAnalyzeAsync::StaticClass()->FindFunctionByName(
            GET_FUNCTION_NAME_CHECKED(ULAMAnalyzeAsync, AnalyzeSoundWaveAsync)));
        A.Finalize();
        Analyze->NodePosX = 320;
        Analyze->FindPinChecked(TEXT("SoundWave"))->DefaultObject =
            LoadObject<USoundWave>(nullptr, TEXT("/Game/Audio/speech_stream.speech_stream"));
        FGraphNodeCreator<UK2Node_CallFunction> P(*Graph);
        auto *Play = P.CreateNode();
        Play->SetFromFunction(ULAMAudio2ExpressionComponent::StaticClass()->FindFunctionByName(
            GET_FUNCTION_NAME_CHECKED(ULAMAudio2ExpressionComponent, PlayExpressionClip)));
        P.Finalize();
        Play->NodePosX = 700;
        bool OK = true;
        if (!Component->GetValuePin())
        {
            UE_LOG(LogTemp, Error, TEXT("LAM example: component variable pin is missing"));
            return false;
        }
        OK &= Schema->TryCreateConnection(Begin->FindPinChecked(UEdGraphSchema_K2::PN_Then),
                                          Analyze->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
        OK &= Schema->TryCreateConnection(Component->GetValuePin(), Analyze->FindPinChecked(TEXT("Component")));
        OK &= Schema->TryCreateConnection(Component->GetValuePin(), Play->FindPinChecked(UEdGraphSchema_K2::PN_Self));
        OK &= Schema->TryCreateConnection(Analyze->FindPinChecked(TEXT("Completed")), Play->GetExecPin());
        OK &= Schema->TryCreateConnection(Analyze->FindPinChecked(TEXT("Clip")), Play->FindPinChecked(TEXT("Clip")));
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        FKismetEditorUtilities::CompileBlueprint(BP);
        if (!OK || BP->Status == BS_Error || !SaveLAMAsset(BP))
            return false;
    }
    if (!LoadObject<UAnimBlueprint>(nullptr, TEXT("/Game/Examples/ABP_LAMCurves.ABP_LAMCurves"), nullptr, LOAD_NoWarn))
    {
        auto *Skeleton =
            LoadObject<USkeleton>(nullptr, TEXT("/Engine/EngineMeshes/SkeletalCube_Skeleton.SkeletalCube_Skeleton"));
        if (!Skeleton)
            return false;
        auto *Factory = NewObject<UAnimBlueprintFactory>();
        Factory->TargetSkeleton = Skeleton;
        auto *Package = CreatePackage(TEXT("/Game/Examples/ABP_LAMCurves"));
        auto *BP = Cast<UAnimBlueprint>(Factory->FactoryCreateNew(
            UAnimBlueprint::StaticClass(), Package, TEXT("ABP_LAMCurves"), RF_Public | RF_Standalone, nullptr, GWarn));
        TArray<UEdGraph *> Graphs;
        BP->GetAllGraphs(Graphs);
        UEdGraph *Graph = nullptr;
        UAnimGraphNode_Root *Root = nullptr;
        for (auto *G : Graphs)
            for (const auto &N : G->Nodes)
                if (auto *R = Cast<UAnimGraphNode_Root>(N.Get()))
                {
                    Graph = G;
                    Root = R;
                }
        if (!Graph || !Root)
            return false;
        FGraphNodeCreator<UAnimGraphNode_LocalRefPose> R(*Graph);
        auto *Ref = R.CreateNode();
        R.Finalize();
        Ref->NodePosX = -500;
        FGraphNodeCreator<UAnimGraphNode_LAMARKit> N(*Graph);
        auto *Node = N.CreateNode();
        N.Finalize();
        Node->NodePosX = -240;
        auto Find = [](UEdGraphNode *O, EEdGraphPinDirection D) -> UEdGraphPin *
        {
            for (auto *Pin : O->Pins)
                if (Pin->Direction == D && Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
                    return Pin;
            return nullptr;
        };
        const auto *Schema = Graph->GetSchema();
        if (!Schema->TryCreateConnection(Find(Ref, EGPD_Output), Node->FindPinChecked(TEXT("SourcePose"))) ||
            !Schema->TryCreateConnection(Find(Node, EGPD_Output), Find(Root, EGPD_Input)))
            return false;
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
        FKismetEditorUtilities::CompileBlueprint(BP);
        if (BP->Status == BS_Error || !SaveLAMAsset(BP))
            return false;
    }
    return true;
}
