#include "Modules/ModuleManager.h"
#include "AssetToolsModule.h"
#include "AssetTypeActions_Base.h"
#include "ContentBrowserModule.h"
#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IDetailsView.h"
#include "LAMBakedExpressionClip.h"
#include "LAMBakeSubsystem.h"
#include "PropertyEditorModule.h"
#include "Sound/SoundWave.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
void ShowBakeWindow(TArray<FAssetData> Assets, bool bRegenerate)
{
    auto *Subsystem = GEditor->GetEditorSubsystem<ULAMBakeSubsystem>();
    if (!Subsystem || Subsystem->IsBusy())
        return;
    auto Options = MakeShared<TStrongObjectPtr<ULAMBakeOptions>>(NewObject<ULAMBakeOptions>());
    FDetailsViewArgs Args;
    Args.bAllowSearch = false;
    Args.bHideSelectionTip = true;
    auto Details =
        FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor")).CreateDetailView(Args);
    Details->SetObject(Options->Get());
    auto Window = SNew(SWindow)
                      .Title(FText::FromString(bRegenerate ? TEXT("Regenerate LAM Expression Clips")
                                                           : TEXT("Generate LAM Expression Clips")))
                      .ClientSize(FVector2D(520, 540))
                      .SupportsMaximize(false)
                      .SupportsMinimize(false);
    TWeakObjectPtr<ULAMBakeSubsystem> WeakSubsystem(Subsystem);
    TWeakPtr<SWindow> WeakWindow(Window);
    auto bStarted = MakeShared<bool>(false);
    auto BatchSerial = MakeShared<uint64>(0);
    Window->SetContent(
        SNew(SVerticalBox) +
        SVerticalBox::Slot().AutoHeight().Padding(
            12)[SNew(STextBlock).Text(FText::FromString(FString::Printf(TEXT("%d selected assets"), Assets.Num())))] +
        SVerticalBox::Slot().FillHeight(1).Padding(
            12)[SNew(SBox)
                    .Visibility(bRegenerate ? EVisibility::Collapsed : EVisibility::Visible)
                    .IsEnabled_Lambda([WeakSubsystem, bStarted]()
                                      { return WeakSubsystem.IsValid() && !*bStarted; })[Details]] +
        SVerticalBox::Slot().AutoHeight().Padding(
            12)[SNew(SProgressBar)
                    .Percent_Lambda(
                        [WeakSubsystem, bStarted]() -> TOptional<float>
                        { return WeakSubsystem.IsValid() && *bStarted ? WeakSubsystem->GetProgress() : 0.f; })] +
        SVerticalBox::Slot().AutoHeight().Padding(
            12)[SNew(STextBlock)
                    .AutoWrapText(true)
                    .Text_Lambda(
                        [WeakSubsystem, bStarted]()
                        {
                            return FText::FromString(WeakSubsystem.IsValid() && *bStarted ? WeakSubsystem->GetStatus()
                                                                                          : FString());
                        })] +
        SVerticalBox::Slot().MaxHeight(120).Padding(
            12)[SNew(SScrollBox) + SScrollBox::Slot()[SNew(STextBlock)
                                                          .AutoWrapText(true)
                                                          .Text_Lambda(
                                                              [WeakSubsystem, bStarted]()
                                                              {
                                                                  return FText::FromString(
                                                                      WeakSubsystem.IsValid() && *bStarted
                                                                          ? FString::Join(WeakSubsystem->Errors,
                                                                                          TEXT("\n"))
                                                                          : FString());
                                                              })]] +
        SVerticalBox::Slot().AutoHeight().Padding(
            12)[SNew(SHorizontalBox) +
                SHorizontalBox::Slot()
                    .AutoWidth()[SNew(SButton)
                                     .Text(FText::FromString(bRegenerate ? TEXT("Regenerate") : TEXT("Generate")))
                                     .IsEnabled_Lambda(
                                         [WeakSubsystem, bStarted]()
                                         {
                                             return WeakSubsystem.IsValid() && !*bStarted && !WeakSubsystem->IsBusy() &&
                                                    !GEditor->PlayWorld;
                                         })
                                     .OnClicked_Lambda(
                                         [WeakSubsystem, Assets, Options, bRegenerate, bStarted, BatchSerial]()
                                         {
                                             if (!WeakSubsystem.IsValid())
                                                 return FReply::Handled();
                                             if (bRegenerate)
                                             {
                                                 TArray<ULAMBakedExpressionClip *> Clips;
                                                 for (const auto &Asset : Assets)
                                                     if (auto *Clip = Cast<ULAMBakedExpressionClip>(Asset.GetAsset()))
                                                         Clips.Add(Clip);
                                                 *bStarted = WeakSubsystem->RegenerateClips(Clips);
                                             }
                                             else
                                             {
                                                 TArray<USoundWave *> Sounds;
                                                 for (const auto &Asset : Assets)
                                                     if (auto *Sound = Cast<USoundWave>(Asset.GetAsset()))
                                                         Sounds.Add(Sound);
                                                 *bStarted =
                                                     WeakSubsystem->GenerateClips(Sounds, Options->Get()->Settings);
                                             }
                                             if (*bStarted)
                                                 *BatchSerial = WeakSubsystem->GetBatchSerial();
                                             return FReply::Handled();
                                         })] +
                SHorizontalBox::Slot().AutoWidth().Padding(
                    8, 0)[SNew(SButton)
                              .Text_Lambda(
                                  [WeakSubsystem, bStarted]()
                                  {
                                      return FText::FromString(WeakSubsystem.IsValid() && *bStarted &&
                                                                       WeakSubsystem->IsBusy()
                                                                   ? TEXT("Cancel")
                                                                   : TEXT("Close"));
                                  })
                              .OnClicked_Lambda(
                                  [WeakSubsystem, WeakWindow, bStarted, BatchSerial]()
                                  {
                                      if (*bStarted && WeakSubsystem.IsValid() && WeakSubsystem->IsBusy() &&
                                          WeakSubsystem->GetBatchSerial() == *BatchSerial)
                                          WeakSubsystem->Cancel();
                                      else if (auto Pinned = WeakWindow.Pin())
                                          Pinned->RequestDestroyWindow();
                                      return FReply::Handled();
                                  })]]);
    Window->SetOnWindowClosed(FOnWindowClosed::CreateLambda(
        [WeakSubsystem, bStarted, BatchSerial](const TSharedRef<SWindow> &)
        {
            if (*bStarted && WeakSubsystem.IsValid() && WeakSubsystem->IsBusy() &&
                WeakSubsystem->GetBatchSerial() == *BatchSerial)
                WeakSubsystem->Cancel();
        }));
    FSlateApplication::Get().AddWindow(Window);
}
class FLAMBakedAssetActions : public FAssetTypeActions_Base
{
  public:
    FText GetName() const override
    {
        return NSLOCTEXT("LAM", "BakedAssetName", "LAM Expression Clip");
    }
    FColor GetTypeColor() const override
    {
        return FColor(40, 170, 130);
    }
    UClass *GetSupportedClass() const override
    {
        return ULAMBakedExpressionClip::StaticClass();
    }
    uint32 GetCategories() override
    {
        return EAssetTypeCategories::Sounds;
    }
};
TSharedRef<FExtender> ExtendMenu(const TArray<FAssetData> &Assets)
{
    auto Extender = MakeShared<FExtender>();
    TArray<FAssetData> Sounds, Clips;
    for (const auto &Asset : Assets)
    {
        if (Asset.AssetClassPath == USoundWave::StaticClass()->GetClassPathName())
            Sounds.Add(Asset);
        if (Asset.AssetClassPath == ULAMBakedExpressionClip::StaticClass()->GetClassPathName())
            Clips.Add(Asset);
    }
    if (!Sounds.IsEmpty() || !Clips.IsEmpty())
        Extender->AddMenuExtension(
            TEXT("GetAssetActions"), EExtensionHook::After, nullptr,
            FMenuExtensionDelegate::CreateLambda(
                [Sounds, Clips](FMenuBuilder &Menu)
                {
                    auto CanBake = FCanExecuteAction::CreateLambda(
                        []()
                        {
                            return GEditor && !GEditor->PlayWorld &&
                                   !GEditor->GetEditorSubsystem<ULAMBakeSubsystem>()->IsBusy();
                        });
                    if (!Sounds.IsEmpty())
                        Menu.AddMenuEntry(
                            NSLOCTEXT("LAM", "GenerateClip", "Generate LAM Expression Clip"),
                            NSLOCTEXT("LAM", "GenerateClipTip",
                                      "Bake ARKit curves into an asset for playback without analysis."),
                            FSlateIcon(),
                            FUIAction(FExecuteAction::CreateLambda([Sounds]() { ShowBakeWindow(Sounds, false); }),
                                      CanBake));
                    if (!Clips.IsEmpty())
                        Menu.AddMenuEntry(
                            NSLOCTEXT("LAM", "RegenerateClip", "Regenerate"),
                            NSLOCTEXT("LAM", "RegenerateClipTip",
                                      "Regenerate using each clip's saved sound and analysis settings."),
                            FSlateIcon(),
                            FUIAction(FExecuteAction::CreateLambda([Clips]() { ShowBakeWindow(Clips, true); }),
                                      CanBake));
                }));
    return Extender;
}
} // namespace
class FLAMEditorModule : public IModuleInterface
{
    TSharedPtr<IAssetTypeActions> Actions;
    FDelegateHandle MenuHandle;

  public:
    void StartupModule() override
    {
        Actions = MakeShared<FLAMBakedAssetActions>();
        FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"))
            .Get()
            .RegisterAssetTypeActions(Actions.ToSharedRef());
        auto Delegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&ExtendMenu);
        MenuHandle = Delegate.GetHandle();
        FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"))
            .GetAllAssetViewContextMenuExtenders()
            .Add(Delegate);
    }
    void ShutdownModule() override
    {
        if (auto *Browser = FModuleManager::GetModulePtr<FContentBrowserModule>(TEXT("ContentBrowser")))
            Browser->GetAllAssetViewContextMenuExtenders().RemoveAll([this](const auto &D)
                                                                     { return D.GetHandle() == MenuHandle; });
        if (auto *Tools = FModuleManager::GetModulePtr<FAssetToolsModule>(TEXT("AssetTools")); Tools && Actions)
            Tools->Get().UnregisterAssetTypeActions(Actions.ToSharedRef());
        Actions.Reset();
    }
};
IMPLEMENT_MODULE(FLAMEditorModule, LAMAudio2ExpressionEditor)
