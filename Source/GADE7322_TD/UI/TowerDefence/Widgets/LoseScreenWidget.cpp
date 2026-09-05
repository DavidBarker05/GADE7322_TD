#include "UI/TowerDefence/Widgets/LoseScreenWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TowerDefencePawns/TowerDefencePawnFactory.h"

bool ULoseScreenWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(RestartButton && QuitButton)) return false;
    RestartButton->OnClicked.AddDynamic(this, &ULoseScreenWidget::RestartGame);
    QuitButton->OnClicked.AddDynamic(this, &ULoseScreenWidget::QuitGame);
    return true;
}

void ULoseScreenWidget::RestartGame() const
{
    if (APlayerController* PC = GetOwningPlayer()) PC->SetPause(false);
    if (TOWER_DEFENCE_PAWN_FACTORY_EXISTS) CLEAR_PAWN_POOLS();
    const FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(this, true);
    UGameplayStatics::OpenLevel(this, FName(*CurrentMapName));
}

void ULoseScreenWidget::QuitGame() const
{
    UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
