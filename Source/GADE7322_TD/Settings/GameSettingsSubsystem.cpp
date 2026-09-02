#include "Settings/GameSettingsSubsystem.h"

#include "Kismet/GameplayStatics.h"

namespace
{
constexpr const TCHAR* GameSettingsSaveSlot = TEXT("GameSettings");
}

void UGameSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if (const UGameSettingsSaveGame* SaveGame =
            Cast<UGameSettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(GameSettingsSaveSlot, 0)))
        bAutoPlay = SaveGame->bAutoPlay;
}

void UGameSettingsSubsystem::SetAutoPlay(bool bNewAutoPlay)
{
    bAutoPlay = bNewAutoPlay;
    if (UGameSettingsSaveGame* SaveGame =
            Cast<UGameSettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(UGameSettingsSaveGame::StaticClass())))
    {
        SaveGame->bAutoPlay = bAutoPlay;
        UGameplayStatics::SaveGameToSlot(SaveGame, GameSettingsSaveSlot, 0);
    }
}
