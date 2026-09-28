#include "LAMEditorLibrary.h"
#include "LAMBakedExpressionClip.h"
#include "LAMAudio2ExpressionComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_Event.h"
#include "K2Node_InputKey.h"
#include "K2Node_CallFunction.h"
#include "K2Node_VariableGet.h"
#include "EdGraphSchema_K2.h"
#include "Sound/SoundWave.h"
#include "Misc/PackageName.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"

bool ULAMEditorLibrary::CreateBakedExample(ULAMBakedExpressionClip *Clip)
{
    if (!IsValid(Clip) || !Clip->GetPlaybackValidationError().IsEmpty())
        return false;
    const FString Path = TEXT("/Game/Examples/BP_LAMBakedPlayback");
    if (FPackageName::DoesPackageExist(Path))
        return true;
    auto *BP = FKismetEditorUtilities::CreateBlueprint(
        AActor::StaticClass(), CreatePackage(*Path), TEXT("BP_LAMBakedPlayback"), BPTYPE_Normal,
        UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
    BP->SimpleConstructionScript->AddNode(
        BP->SimpleConstructionScript->CreateNode(ULAMAudio2ExpressionComponent::StaticClass(), TEXT("LAM")));
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    auto *Graph = FBlueprintEditorUtils::FindEventGraph(BP);
    const auto *Schema = GetDefault<UEdGraphSchema_K2>();
    FGraphNodeCreator<UK2Node_Event> BeginCreator(*Graph);
    auto *Begin = BeginCreator.CreateNode();
    Begin->EventReference.SetExternalMember(TEXT("ReceiveBeginPlay"), AActor::StaticClass());
    Begin->bOverrideFunction = true;
    BeginCreator.Finalize();
    FGraphNodeCreator<UK2Node_CallFunction> PrimeCreator(*Graph);
    auto *Prime = PrimeCreator.CreateNode();
    Prime->SetFromFunction(UGameplayStatics::StaticClass()->FindFunctionByName(TEXT("PrimeSound")));
    PrimeCreator.Finalize();
    Prime->NodePosX = 320;
    Prime->FindPinChecked(TEXT("InSound"))->DefaultObject = Clip->SoundWave;
    FGraphNodeCreator<UK2Node_InputKey> KeyCreator(*Graph);
    auto *Key = KeyCreator.CreateNode();
    Key->InputKey = EKeys::SpaceBar;
    KeyCreator.Finalize();
    Key->NodePosY = 240;
    FGraphNodeCreator<UK2Node_VariableGet> ComponentCreator(*Graph);
    auto *Component = ComponentCreator.CreateNode();
    Component->VariableReference.SetSelfMember(TEXT("LAM"));
    ComponentCreator.Finalize();
    Component->NodePosY = 440;
    FGraphNodeCreator<UK2Node_CallFunction> PlayCreator(*Graph);
    auto *Play = PlayCreator.CreateNode();
    Play->SetFromFunction(ULAMAudio2ExpressionComponent::StaticClass()->FindFunctionByName(TEXT("PlayExpressionClip")));
    PlayCreator.Finalize();
    Play->NodePosX = 320;
    Play->NodePosY = 240;
    Play->FindPinChecked(TEXT("Clip"))->DefaultObject = Clip;
    bool OK = Component->GetValuePin() != nullptr;
    OK &= Schema->TryCreateConnection(Begin->FindPinChecked(UEdGraphSchema_K2::PN_Then), Prime->GetExecPin());
    OK &= Schema->TryCreateConnection(Key->FindPinChecked(TEXT("Pressed")), Play->GetExecPin());
    if (Component->GetValuePin())
        OK &= Schema->TryCreateConnection(Component->GetValuePin(), Play->FindPinChecked(UEdGraphSchema_K2::PN_Self));
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(BP);
    FKismetEditorUtilities::CompileBlueprint(BP);
    if (!OK || BP->Status == BS_Error)
        return false;
    CastChecked<AActor>(BP->GeneratedClass->GetDefaultObject())->AutoReceiveInput = EAutoReceiveInput::Player0;
    FAssetRegistryModule::AssetCreated(BP);
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public | RF_Standalone;
    return UPackage::SavePackage(
        BP->GetOutermost(), BP,
        *FPackageName::LongPackageNameToFilename(Path, FPackageName::GetAssetPackageExtension()), Args);
}
