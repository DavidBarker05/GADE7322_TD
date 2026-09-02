#pragma once

#include "CoreMinimal.h"

#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "GameSettingsSubsystem.generated.h"

UCLASS()
class GADE7322_TD_API UGameSettingsSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    bool bAutoPlay = false;
};

// Player preferences that persist between play sessions
UCLASS()
class GADE7322_TD_API UGameSettingsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Settings")
    void SetAutoPlay(bool bNewAutoPlay);

    UFUNCTION(BlueprintPure, Category = "Settings")
    bool IsAutoPlay() const { return bAutoPlay; }

private:
    bool bAutoPlay = false;
};
