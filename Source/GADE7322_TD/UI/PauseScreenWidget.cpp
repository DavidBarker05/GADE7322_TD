#include "UI/PauseScreenWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/GameSettingsSubsystem.h"
#include "TowerDefencePlayerController.h"
#include "UI/ControlsWidget.h"

bool UPauseScreenWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(ResumeButton && AutoPlayToggle && RestartButton && ControlsButton && ControlsWidget && QuitButton))
        return false;
    ResumeButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::ResumeGame);
    ControlsButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::OpenControls);
    QuitButton->OnClicked.AddDynamic(this, &UPauseScreenWidget::QuitGame);
    AutoPlayToggle->OnCheckStateChanged.AddDynamic(this, &UPauseScreenWidget::HandleAutoPlayChanged);
    if (const UWorld* World = GetWorld(); const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr)
        if (const UGameSettingsSubsystem* Settings = GameInstance->GetSubsystem<UGameSettingsSubsystem>())
            AutoPlayToggle->SetIsChecked(Settings->IsAutoPlay());
    ShowMainPanel();
    return true;
}

void UPauseScreenWidget::ShowMainPanel() const
{
    ResumeButton->SetVisibility(ESlateVisibility::Visible);
    AutoPlayToggle->SetVisibility(ESlateVisibility::Visible);
    RestartButton->SetVisibility(ESlateVisibility::Visible);
    ControlsButton->SetVisibility(ESlateVisibility::Visible);
    QuitButton->SetVisibility(ESlateVisibility::Visible);
    ControlsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UPauseScreenWidget::ResumeGame() const
{
    if (ATowerDefencePlayerController* TDPC = GetOwningPlayer<ATowerDefencePlayerController>()) TDPC->TogglePause();
}

void UPauseScreenWidget::RestartGame() const
{
    const FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(this, true);
    UGameplayStatics::OpenLevel(this, FName(*CurrentMapName));
}

void UPauseScreenWidget::OpenControls() const
{
    ResumeButton->SetVisibility(ESlateVisibility::Collapsed);
    AutoPlayToggle->SetVisibility(ESlateVisibility::Collapsed);
    RestartButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsButton->SetVisibility(ESlateVisibility::Collapsed);
    QuitButton->SetVisibility(ESlateVisibility::Collapsed);
    ControlsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UPauseScreenWidget::QuitGame() const
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UPauseScreenWidget::HandleAutoPlayChanged(bool bIsChecked) const
{
    if (const UWorld* World = GetWorld(); UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr)
        if (UGameSettingsSubsystem* Settings = GameInstance->GetSubsystem<UGameSettingsSubsystem>())
            Settings->SetAutoPlay(bIsChecked);
}
