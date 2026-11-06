#include "UI/TowerDefence/Widgets/PauseScreenWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/TowerDefence/TowerDefencePlayerController.h"
#include "Settings/GameSettingsSubsystem.h"
#include "TowerDefencePawns/ProjectilePoolFactory.h"
#include "TowerDefencePawns/TowerDefencePawnFactory.h"
#include "UI/TowerDefence/TowerDefenceHUD.h"
#include "UI/Widgets/ControlsWidget.h"
#include "UI/Widgets/GuideWidget.h"

bool UPauseScreenWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(ResumeButton && AutoPlayToggle && RestartButton && GuideButton && GuideWidget && ControlsButton &&
          ControlsWidget && MenuButton && QuitButton))
        return false;
    ResumeButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::ResumeGame);
    RestartButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::RestartGame);
    GuideButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::OpenGuide);
    ControlsButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::OpenControls);
    MenuButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::LoadMenu);
    QuitButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::QuitGame);
    AutoPlayToggle->OnCheckStateChanged.AddDynamic(this, &UPauseScreenWidget::HandleAutoPlayChanged);
    if (const UWorld* World = GetWorld();
        const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr)
        if (const UGameSettingsSubsystem* Settings = GameInstance->GetSubsystem<UGameSettingsSubsystem>())
            AutoPlayToggle->SetIsChecked(Settings->IsAutoPlay());
    GuideWidget->OnBackClicked.AddDynamic(this, &UPauseScreenWidget::ShowMainPanel);
    ControlsWidget->OnBackClicked.AddDynamic(this, &UPauseScreenWidget::ShowMainPanel);
    ShowMainPanel();
    return true;
}

void UPauseScreenWidget::ShowMainPanel() const
{
    ResumeButton->SetVisibility(ESlateVisibility::Visible);
    AutoPlayToggle->SetVisibility(ESlateVisibility::Visible);
    RestartButton->SetVisibility(ESlateVisibility::Visible);
    ControlsButton->SetVisibility(ESlateVisibility::Visible);
    MenuButton->SetVisibility(ESlateVisibility::Visible);
    QuitButton->SetVisibility(ESlateVisibility::Visible);
    GuideWidget->SetVisibility(ESlateVisibility::Collapsed);
    ControlsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UPauseScreenWidget::ResumeGame() const
{
    if (ATowerDefencePlayerController* TD_PC = GetOwningPlayer<ATowerDefencePlayerController>()) TD_PC->TogglePause();
}

void UPauseScreenWidget::RestartGame() const
{
    if (ATowerDefencePlayerController* TD_PC = GetOwningPlayer<ATowerDefencePlayerController>())
    {
        TD_PC->SetPause(false);
        if (const ATowerDefenceHUD* TD_HUD = TD_PC->GetHUD<ATowerDefenceHUD>()) TD_HUD->ShowLoadingScreen();
    }
    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS) CLEAR_PAWN_POOLS();
    if (PROJECTILE_POOL_FACTORY_EXISTS) CLEAR_PROJECTILE_POOLS();
    const FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(this, true);
    UGameplayStatics::OpenLevel(this, FName(*CurrentMapName));
}

void UPauseScreenWidget::OpenGuide() const
{
    ResumeButton->SetVisibility(ESlateVisibility::Collapsed);
    AutoPlayToggle->SetVisibility(ESlateVisibility::Collapsed);
    RestartButton->SetVisibility(ESlateVisibility::Collapsed);
    GuideButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsButton->SetVisibility(ESlateVisibility::Collapsed);
    MenuButton->SetVisibility(ESlateVisibility::Collapsed);
    QuitButton->SetVisibility(ESlateVisibility::Collapsed);
    GuideWidget->SetVisibility(ESlateVisibility::Visible);
}

void UPauseScreenWidget::OpenControls() const
{
    ResumeButton->SetVisibility(ESlateVisibility::Collapsed);
    AutoPlayToggle->SetVisibility(ESlateVisibility::Collapsed);
    RestartButton->SetVisibility(ESlateVisibility::Collapsed);
    GuideButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsButton->SetVisibility(ESlateVisibility::Collapsed);
    MenuButton->SetVisibility(ESlateVisibility::Collapsed);
    QuitButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UPauseScreenWidget::LoadMenu() const
{
    if (MainMenu.IsNull())
    {
        TD_LOG_ERROR(TEXT("No main menu to load"));
        return;
    }
    if (ATowerDefencePlayerController* TD_PC = GetOwningPlayer<ATowerDefencePlayerController>())
    {
        TD_PC->SetPause(false);
        if (const ATowerDefenceHUD* TD_HUD = TD_PC->GetHUD<ATowerDefenceHUD>()) TD_HUD->ShowLoadingScreen();
    }
    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS) CLEAR_PAWN_POOLS();
    if (PROJECTILE_POOL_FACTORY_EXISTS) CLEAR_PROJECTILE_POOLS();
    UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenu);
}

void UPauseScreenWidget::QuitGame() const
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UPauseScreenWidget::HandleAutoPlayChanged(bool bIsChecked) const
{
    if (const UWorld* World = GetWorld();
        const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr)
        if (UGameSettingsSubsystem* Settings = GameInstance->GetSubsystem<UGameSettingsSubsystem>())
            Settings->SetAutoPlay(bIsChecked);
}
