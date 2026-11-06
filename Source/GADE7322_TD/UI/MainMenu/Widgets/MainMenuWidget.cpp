#include "UI/MainMenu/Widgets/MainMenuWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenu/MainMenuHUD.h"
#include "UI/MainMenu/Widgets/CreditsWidget.h"
#include "UI/Widgets/ControlsWidget.h"
#include "UI/Widgets/GuideWidget.h"

bool UMainMenuWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(Title && PlayButton && GuideButton && GuideWidget && ControlsButton && ControlsWidget && CreditsButton &&
          CreditsWidget && QuitButton))
        return false;
    PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::LoadGame);
    GuideButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OpenGuide);
    ControlsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OpenControls);
    CreditsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OpenCredits);
    QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::QuitGame);
    GuideWidget->OnBackClicked.AddDynamic(this, &UMainMenuWidget::ShowMainPanel);
    ControlsWidget->OnBackClicked.AddDynamic(this, &UMainMenuWidget::ShowMainPanel);
    CreditsWidget->OnBackClicked.AddDynamic(this, &UMainMenuWidget::ShowMainPanel);
    ShowMainPanel();
    return true;
}

void UMainMenuWidget::ShowMainPanel() const
{
    Title->SetVisibility(ESlateVisibility::Visible);
    PlayButton->SetVisibility(ESlateVisibility::Visible);
    GuideButton->SetVisibility(ESlateVisibility::Visible);
    ControlsButton->SetVisibility(ESlateVisibility::Visible);
    CreditsButton->SetVisibility(ESlateVisibility::Visible);
    QuitButton->SetVisibility(ESlateVisibility::Visible);
    GuideWidget->SetVisibility(ESlateVisibility::Collapsed);
    ControlsWidget->SetVisibility(ESlateVisibility::Collapsed);
    CreditsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UMainMenuWidget::LoadGame() const
{
    if (MainLevel.IsNull())
    {
        TD_LOG_ERROR(TEXT("No game to load"));
        return;
    }
    if (APlayerController* PC = GetOwningPlayer<APlayerController>())
        if (const AMainMenuHUD* MM_HUD = PC->GetHUD<AMainMenuHUD>()) MM_HUD->ShowLoadingScreen();
    UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainLevel);
}

void UMainMenuWidget::OpenGuide() const
{
    Title->SetVisibility(ESlateVisibility::Collapsed);
    PlayButton->SetVisibility(ESlateVisibility::Collapsed);
    GuideButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsButton->SetVisibility(ESlateVisibility::Collapsed);
    CreditsButton->SetVisibility(ESlateVisibility::Collapsed);
    QuitButton->SetVisibility(ESlateVisibility::Collapsed);
    GuideWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::OpenControls() const
{
    Title->SetVisibility(ESlateVisibility::Collapsed);
    PlayButton->SetVisibility(ESlateVisibility::Collapsed);
    GuideButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsButton->SetVisibility(ESlateVisibility::Collapsed);
    CreditsButton->SetVisibility(ESlateVisibility::Collapsed);
    QuitButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::OpenCredits() const
{
    Title->SetVisibility(ESlateVisibility::Collapsed);
    PlayButton->SetVisibility(ESlateVisibility::Collapsed);
    GuideButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsButton->SetVisibility(ESlateVisibility::Collapsed);
    CreditsButton->SetVisibility(ESlateVisibility::Collapsed);
    QuitButton->SetVisibility(ESlateVisibility::Collapsed);
    CreditsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::QuitGame() const
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
