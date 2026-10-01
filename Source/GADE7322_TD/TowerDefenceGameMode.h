#pragma once

#include "CoreMinimal.h"

#include "Events/EventListener.h"
#include "GameFramework/GameModeBase.h"
#include "GenericTeamAgentInterface.h"
#include "TowerDefencePawns/TowerDefencePawn.h"

#include "TowerDefenceGameMode.generated.h"

class AAttacker;
class APlayerTower;
class AProceduralTerrainGen;

USTRUCT(BlueprintType)
struct FEnemySpawnEntry
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemies")
    TSubclassOf<AAttacker> EnemyClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemies", meta = (ClampMin = 1, UIMin = 1))
    int32 MinWave = 1;
};

UCLASS()
class GADE7322_TD_API ATowerDefenceGameMode : public AGameModeBase,
                                              public IEventListener
{
    GENERATED_BODY()

    EVENTS_TO_LISTEN_TO(TEXT("DeathEvent"), TEXT("DefenderPurchasedEvent"), TEXT("DefenderSoldEvent"),
                        TEXT("LeakEvent"))

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void StartPlay() override;

    UFUNCTION(BlueprintCallable)
    virtual void OnEventReceived_Implementation(const FName& EventName, const TArray<FAny>& Params) override;

    UFUNCTION(BlueprintCallable, Category = "Waves")
    void StartNextWave();

    bool IsWaveInProgress() const { return bWaveInProgress; }

    int32 GetCurrentWave() const { return CurrentWave; }

    int32 GetEnemiesRemaining() const { return EnemiesLeftToSpawnThisWave + EnemiesAliveThisWave; }

    static ETeamAttitude::Type GetAttitudeCustom(EAITeam TeamA, EAITeam TeamB);

    static ETeamAttitude::Type GetAttitude(FGenericTeamId TeamA, FGenericTeamId TeamB);

    UFUNCTION(BlueprintCallable, Category = "Adaptive Difficulty")
    void DefenderLostHealth(int32 Amount)
    {
        if (Amount > 0) HealthLost += Amount;
    }

    UFUNCTION(BlueprintCallable, Category = "Adaptive Difficulty")
    void DefenderGainedHealth(int32 Amount)
    {
        if (Amount > 0) HealthGained += Amount;
    }

protected:
    void SpawnBurst();

    void SpawnEnemyOnRandomPath();

    void HandleEnemyDeath(AAttacker* Enemy);

    void HandleTowerDeath(APlayerTower* Tower);

    void CheckWaveComplete();

    int32 GetEnemyCountForWave(int32 Wave) const
    {
        const int32 BaseCount = BaseEnemiesPerWave + EnemiesPerWaveGrowth * FMath::Max(0, Wave - 1);
        return FMath::Max(1, FMath::RoundToInt32(BaseCount + CumulativeDifficultyScore * EnemyCountScoreInfluence));
    }

    int32 GetGoldRewardForWave(int32 Wave) const;

    void BroadcastEnemyCount() const;

    void UpdateCumulativeDifficultyScore();

    float GetStrengthMultiplier() const
    {
        if (CumulativeDifficultyScore > StrengthScoreThreshold)
        {
            const float ScoreAboveThreshold = CumulativeDifficultyScore - StrengthScoreThreshold;
            return FMath::Min(1.0f + ScoreAboveThreshold * StrengthScoreInfluence, MaxStrengthMultiplier);
        }
        if (CumulativeDifficultyScore < -StrengthScoreThreshold)
        {
            const float ScoreBelowThreshold = -StrengthScoreThreshold - CumulativeDifficultyScore;
            return FMath::Max(1.0f - ScoreBelowThreshold * StrengthScoreInfluence, MinStrengthMultiplier);
        }
        return 1.0f;
    }

    // Enemy types that can spawn, each gated by a minimum wave number
    // One is picked at random, from among those unlocked for CurrentWave, for each spawn
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemies", meta = (AllowPrivateAccess = true))
    TArray<FEnemySpawnEntry> EnemyClasses;

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

    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float SurvivalFactorInfluence = 1.0f;
    int32 NumAliveDefenders = 0;
    int32 NumTotalDefenders = 0;
    float GetSurvivalFactor() const
    {
        if (NumTotalDefenders == 0) return -1.0f;
        const float SurvivalRate = static_cast<float>(NumAliveDefenders) / static_cast<float>(NumTotalDefenders);
        const float SurvivalFactor = 2.0f * SurvivalRate - 1.0f;
        return SurvivalFactor * SurvivalFactorInfluence;
    }

    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float LeakFactorInfluence = 1.0f;
    int32 NumLeakedEnemies = 0;
    int32 NumTotalEnemies = 0;
    float GetLeakFactor() const
    {
        if (NumTotalEnemies == 0) return 1.0f;
        const float LeakRate = static_cast<float>(NumLeakedEnemies) / static_cast<float>(NumTotalEnemies);
        const float LeakFactor = 1.0f - 2.0f * LeakRate;
        return LeakFactor * LeakFactorInfluence;
    }

    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float HealthFactorInfluence = 1.0f;
    int32 HealthGained = 0;
    int32 HealthLost = 0;
    int32 HealthTotal = 0;
    float GetHealthFactor() const
    {
        if (HealthTotal == 0) return -1.0f;
        const float HealthRatio = static_cast<float>(HealthGained - HealthLost) / static_cast<float>(HealthTotal);
        const float HealthFactor = FMath::Clamp(HealthRatio, -1.0f, 1.0f);
        return HealthFactor * HealthFactorInfluence;
    }

    float GetRoundScore() const { return GetSurvivalFactor() + GetLeakFactor() + GetHealthFactor(); }

    float CumulativeDifficultyScore = 0.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
    float DifficultyScoreDecay = 0.6f;

    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float DifficultyScoreCap = 6.0f;

    // How many extra/fewer enemies to spawn per point of CumulativeDifficultyScore
    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float EnemyCountScoreInfluence = 1.0f;

    // CumulativeDifficultyScore must exceed this before enemy health/damage starts scaling up at all
    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float StrengthScoreThreshold = 1.5f;

    // How much GetStrengthMultiplier() rises per point of score above StrengthScoreThreshold
    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, UIMin = 0.0))
    float StrengthScoreInfluence = 0.1f;

    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 1.0, UIMin = 1.0))
    float MaxStrengthMultiplier = 1.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Adaptive Difficulty",
              meta = (AllowPrivateAccess = true, ClampMin = 0.0, ClampMax = 1.0, UIMin = 0.0, UIMax = 1.0))
    float MinStrengthMultiplier = 0.6f;

    FTimerHandle SpawnBurstTimerHandle;
};
