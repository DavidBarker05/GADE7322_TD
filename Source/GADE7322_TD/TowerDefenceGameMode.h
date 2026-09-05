#pragma once

#include "CoreMinimal.h"

#include "Events/EventListener.h"
#include "GameFramework/GameModeBase.h"
#include "GenericTeamAgentInterface.h"

#include "TowerDefenceGameMode.generated.h"

class AAttacker;
class APlayerTower;
class AProceduralTerrainGen;

UCLASS()
class GADE7322_TD_API ATowerDefenceGameMode : public AGameModeBase,
                                              public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("DeathEvent"))

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void StartPlay() override;

    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

    // Called by the HUD's Start/Next Wave button. Does nothing while a wave is still in progress
    UFUNCTION(BlueprintCallable, Category = "Waves")
    void StartNextWave();

    bool IsWaveInProgress() const { return bWaveInProgress; }

    int32 GetCurrentWave() const { return CurrentWave; }

    int32 GetEnemiesRemaining() const { return EnemiesLeftToSpawnThisWave + EnemiesAliveThisWave; }

    static ETeamAttitude::Type GetAttitude(FGenericTeamId TeamA, FGenericTeamId TeamB);

protected:
    // Spawns one burst of enemies, then (if there are more left to spawn this wave) schedules the next burst
    void SpawnBurst();

    void SpawnEnemyOnRandomPath();

    void HandleEnemyDeath(AAttacker* Enemy);

    void HandleTowerDeath(APlayerTower* Tower);

    void CheckWaveComplete();

    int32 GetEnemyCountForWave(int32 Wave) const;

    int32 GetGoldRewardForWave(int32 Wave) const;

    void BroadcastEnemyCount() const;

    // Enemy pawn classes that can spawn - one is picked at random for each spawn
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", meta = (AllowPrivateAccess = true))
    TArray<TSubclassOf<AAttacker>> EnemyClasses;

    // Number of enemies in wave 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Waves", meta = (AllowPrivateAccess = true, ClampMin = 1))
    int32 BaseEnemiesPerWave = 5;

    // Extra enemies added for every wave beyond the first
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Waves", meta = (AllowPrivateAccess = true, ClampMin = 0))
    int32 EnemiesPerWaveGrowth = 2;

    // How many enemies spawn together in each burst
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Waves", meta = (AllowPrivateAccess = true, ClampMin = 1))
    int32 EnemiesPerBurst = 3;

    // Delay between bursts of enemies spawning within a wave
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Waves",
              meta = (AllowPrivateAccess = true, ClampMin = 0.1, UIMin = 0.1, Units = "Seconds"))
    float BurstInterval = 1.5f;

    // Gold reward for clearing wave 1
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Waves", meta = (AllowPrivateAccess = true, ClampMin = 0))
    int32 BaseWaveClearReward = 50;

    // Extra gold reward added for every wave beyond the first
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Waves", meta = (AllowPrivateAccess = true, ClampMin = 0))
    int32 WaveClearRewardGrowth = 10;

private:
    UPROPERTY()
    AProceduralTerrainGen* TerrainGen = nullptr;

    int32 CurrentWave = 0;
    int32 EnemiesLeftToSpawnThisWave = 0;
    int32 EnemiesAliveThisWave = 0;
    bool bWaveInProgress = false;

    FTimerHandle SpawnBurstTimerHandle;
};
